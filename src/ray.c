#include <math.h>

#include "common.h"
#include "gfx.h"
#include "player.h"
#include "scene.h"
#include "map.h"
#include "geometry.h"
#include "viz.h"
#include "render.h"
#include "pixel.h"
#include "critter.h"

#include "ray.h"

Ray ray_invert(Ray r) {
  r.dir = point_invert(r.dir);
  r.end = point_invert(r.end);
  return r;
}

Ray create_ray() {
  return (Ray) {
    .start = g_player.body.pos,
    .end = g_player.body.pos,
    .dir = g_player.body.dir,
    .dist = DAMN_NEAR_INFINITY,
    .hit = RAY_HIT_OOB,
    .pixel = POINT_OOB,
  };
}

Point ray_cast_step_point(Ray r) {
  Point d;

  d.x = r.dir.x > 0 ?
    ceil(r.end.x) + 0.0001 - r.end.x:
    floor(r.end.x) - 0.0001 - r.end.x;

  d.y = d.x * (r.dir.y / r.dir.x);
  return point_add(r.end, d);
}

Point ray_cast_step_point_inv(Ray r) {
  return point_invert(ray_cast_step_point(ray_invert(r)));
}

void ray_scan() {
  Ray r, r_tmp;

  for (int col = 0; col <= SCREEN_W; col++)
  {
    r = create_ray();
    r.pixel.x = col;

    // Set the direction of the ray based on the camera properties and current column
    float camera_x = (2.0 * ((float)col / SCREEN_W)) - 1.0;
    Point camera_plane = point_mult(g_player.camera_plane, FOCAL_LENGTH * camera_x);

    r.dir = point_add(r.dir, camera_plane);
    r.start = r.end = g_player.body.pos;

#ifndef VIZ_WALL_RAY
    viz_map_vector(r.start, camera_plane, COLOR_CYAN);
    viz_map_line(r.start, r.end, COLOR_YELLOW);
    viz_map_dot(r.end, 8, COLOR_GREEN);
#endif

    float closest_d_sq = DAMN_NEAR_INFINITY;
    Ray r_h, r_v = r;
    for (int i = 0; i < MAP_TILES_X; i++) {
      r_h.end = ray_cast_step_point(r);
      r_v.end = ray_cast_step_point_inv(r);

#ifndef VIZ_WALL_RAY
      viz_map_dot(r_h.end, 5, COLOR_CYAN);
      viz_map_dot(r_v.end, 5, COLOR_MAGENTA);
#endif

      float d_sq_h = point_dist_squared(r.start, r_h.end);
      float d_sq_v = point_dist_squared(r.start, r_v.end);

      r.end = d_sq_h < d_sq_v ? r_h.end : r_v.end;
      r.hit.tile = map_tile_at_point(r.end);
      if (map_tile_is_wall(r.hit.tile)) {
        if (d_sq_h < d_sq_v) {
          // The perpendicular distance avoids fisheye
          r.dist = (r.end.x - r.start.x) / r.dir.x;
          // Local wall coordinates of hit
          r.hit.local.x = (point_fractional(r.end)).y;
        }
        else {
          // The perpendicular distance avoids fisheye
          r.dist = (r.end.y - r.start.y) / r.dir.y;
          // Local wall coordinates of hit
          r.hit.local.x = (point_fractional(r.end)).x;
        }
        break;
      }
    }

    // Calculate the wall top and bottom.
    float wall_h = (WALL_H * SCREEN_H) / (r.dist);
    float wall_top = SCREEN_HORIZON - (wall_h / 2.0);
    float wall_bot = wall_top + wall_h;

    // Draw each row top to bottom
    for(int row = 0; row < SCREEN_H; row++)
    {
      Ray r_y = r;
      r.pixel.y = row;

      r.hit.type = (row < wall_top) ? HIT_CEIL  : HIT_WALL;
      r.hit.type = (row > wall_bot) ? HIT_FLOOR : r.hit.type;

      Point vr = point_sub(r.end, r.start);
      for (int i = 0; i < MAX_CRITTERS; i++) {
        float radius = g_critters[i].body.radius;
        Point c = g_critters[i].body.pos,
              dir = point_sub(c, r.start);
        float billboad_dir = point_dot(vr, dir);
    
        // a->b dot a->c is positive if the player is facing towards the billboard.
        float circle_dist_sq = point_dist_squared(r.start, c);
        if (circle_dist_sq < closest_d_sq && billboad_dir > 0) {
          float circle_dist = sqrt(circle_dist_sq);
          // Once per frame per critter
          int critter_h = (WALL_H * SCREEN_H) / circle_dist;
          int critter_top = SCREEN_HORIZON - (critter_h / 2);

          if (r.pixel.y > critter_top && r.pixel.y < critter_top + critter_h) {
            // Once per frame per critter
            Point billboard_plane = point_mult(g_player.camera_plane, radius);
            Point d = point_add(c, billboard_plane);
            Point e = point_sub(c, billboard_plane);
            Point vs = point_sub(d, e);
            Point vq = point_sub(r.start, e);

            float u = point_cross(vq, vr) / point_cross(vs, vr);

#ifndef VIZ_RAY_CRITTER
            viz_map_line(e, d, COLOR_MAGENTA);
            viz_map_vector(e, vq, COLOR_RED);
            viz_map_vector(r.start, vr, COLOR_YELLOW);
            viz_map_vector(e, vs, COLOR_BLUE);
#endif

            if (u > 0 && u <= 1) {
              r.hit.local.x = u;
              r.hit.local.y = (r.pixel.y - critter_top) / critter_h;
              r.dist = sqrt(circle_dist_sq);
              r.end = point_add(e, point_mult(point_sub(d, e), u));

              r.hit.type = HIT_CRITTER;
              viz_map_ray_critter_hit(r);
            }
          }
        }
      }

      if (r.hit.type == HIT_CEIL || r.hit.type == HIT_FLOOR) {
        r.dist = fabs(CAMERA_HEIGHT / (row - SCREEN_HORIZON));
        r.end = point_add(g_player.body.pos, point_mult(r.dir, r.dist));
        r.hit.local = point_fractional(r.end);
      }
      else if (r.hit.type == HIT_WALL)
      {
        r.hit.local.y = (float)(row - wall_top) / wall_h;
      }
      render_ray(r);
    }
  }
}