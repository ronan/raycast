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
    .z = 0.0f
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
  Ray r;

  Point ray_dir_from = point_dir(g_player.camera_plane, g_player.body.dir);
  float view_cone_limit = point_dot(g_player.body.dir, ray_dir_from);

  // Pre-process each critter outside of the inner-loops
  for (int i = 0; i < MAX_CRITTERS; i++) {
    g_critters[i].camera_dist = DAMN_NEAR_INFINITY;
    g_critters[i].screen_h = g_critters[i].screen_top = g_critters[i].screen_bot = 0;

    Point dir = point_dir(g_camera_pos, g_critters[i].body.pos);
    g_critters[i].camera_ang_cos = point_dot(dir, g_camera_dir);
    if (g_critters[i].camera_ang_cos > view_cone_limit) {
      Point billboard_plane = point_mult(g_camera_plane, g_critters[i].body.radius);
      g_critters[i].camera_dist    = point_dist(g_camera_pos, g_critters[i].body.pos);
      g_critters[i].camera_right   = point_add(g_critters[i].body.pos, billboard_plane);
      g_critters[i].camera_left    = point_sub(g_critters[i].body.pos, billboard_plane);

      // Todo clean up this mess:
      Point p = g_critters[i].body.pos;
      Point l1 = g_camera_pos;
      Point l2 = point_add(g_camera_pos, g_camera_plane);

      float u = (((p.x - l1.x) * (l2.x - l1.x)) + ((p.y - l1.y)* (l2.y - l1.y)));
      Point x = point_add(g_camera_pos, point_mult(g_camera_plane, u));
      g_critters[i].camera_dist = point_dist(g_critters[i].body.pos, x);

      float wall_h = SCREEN_H / g_critters[i].camera_dist;
      float wall_top = SCREEN_HORIZON - (wall_h / 2.0);
      float wall_bot = wall_top + wall_h;

      g_critters[i].screen_h   = (g_critters[i].body.height * SCREEN_H) / g_critters[i].camera_dist;
      g_critters[i].screen_bot = wall_bot;
      g_critters[i].screen_top = g_critters[i].screen_bot - g_critters[i].screen_h;
    }
  }

  // Sort the critters by distance. Insertion sort works well on almost sorted arrays
  for (int i = 0, j = -1; i < MAX_CRITTERS; i++, j++) {
    Critter c = g_critters[i];
    
    while (j >= 0 && g_critters[j].camera_dist > c.camera_dist) {
      g_critters[j + 1] = g_critters[j];
      g_critters[j] = c;
      j--;
    }
  }

  for (int col = 0; col <= SCREEN_W; col++)
  {
    r = create_ray();
    r.pixel.x = col;
    r.hit.type = HIT_WALL;

    // Set the direction of the ray based on the camera properties and current column
    float camera_x = (2.0 * (r.pixel.x / SCREEN_W)) - 1.0;
    Point camera_plane = point_mult(g_player.camera_plane, FOV * camera_x);

    r.dir = point_add(r.dir, camera_plane);
    r.start = r.end = g_player.body.pos;

#ifndef NOVIZ_RAY_WALL
    viz_map_vector(r.start, camera_plane, COLOR_CYAN);
    viz_map_vector(r.start, point_mult(r.dir, 10), COLOR_YELLOW);
#endif

    float closest_d_sq = DAMN_NEAR_INFINITY;
    Ray r_h, r_v = r;
    for (int i = 0; i < MAP_TILES_X * 2; i++) {
      r_h.end = ray_cast_step_point(r);
      r_v.end = ray_cast_step_point_inv(r);

#ifndef NOVIZ_RAY_WALL
      viz_map_dot(r_h.end, 5, COLOR_CYAN);
      viz_map_dot(r_v.end, 5, COLOR_MAGENTA);
#endif

      float d_sq_h = point_dist_squared(r.start, r_h.end);
      float d_sq_v = point_dist_squared(r.start, r_v.end);

      r.end = d_sq_h < d_sq_v ? r_h.end : r_v.end;
      r.hit.tile = map_tile_at_point(r.end);
      if (map_tile_is_wall(r.hit.tile)) {
        if (d_sq_h < d_sq_v) {
          // The closest wall or critter at this column
          closest_d_sq = d_sq_v;
          // The perpendicular distance avoids fisheye
          r.dist = (r.end.x - r.start.x) / r.dir.x;
          // Local wall coordinates of hit
          r.hit.local.x = (point_fractional(r.end)).y;
        }
        else {
          closest_d_sq = d_sq_h;
          // The perpendicular distance avoids fisheye
          r.dist = (r.end.y - r.start.y) / r.dir.y;
          // Local wall coordinates of hit
          r.hit.local.x = (point_fractional(r.end)).x;
        }
        break;
      }
    }

    // Calculate the wall top and bottom.
    float wall_h = SCREEN_H / r.dist;
    float wall_top = SCREEN_HORIZON - (wall_h / 2.0);
    float wall_bot = wall_top + wall_h;
    float wall_dist = sqrt(closest_d_sq);

    int critter_hit_count = 0;
    CritterHit critter_hits[16];

    Ray r_critter = r;
    Point vr = point_sub(r.end, r.start);
    float critter_dist = wall_dist;
    for (int i = 0; i < MAX_CRITTERS; i++) {
      // Critters are sorted by distance so we can stop looking when we reach a wall
      if (g_critters[i].camera_dist > wall_dist) {
        break;
      }

      Point d = g_critters[i].camera_right;
      Point e = g_critters[i].camera_left;
      Point vs = point_sub(d, e);
      Point vq = point_sub(r.start, e);

      float u = point_cross(vq, vr) / point_cross(vs, vr);

#ifndef NOVIZ_RAY_CRITTER
      viz_map_line(e, d, COLOR_MAGENTA);
      viz_map_vector(e, vq, COLOR_RED);
      viz_map_vector(r.start, vr, COLOR_YELLOW);
      viz_map_vector(e, vs, COLOR_BLUE);
#endif

      if (u > 0 && u <= 1) {
        r_critter.hit.local.x = u;
        r_critter.end = point_add(e, point_mult(point_sub(d, e), u));
        critter_dist = r_critter.dist = g_critters[i].camera_dist;
        
        critter_hits[critter_hit_count].r = r_critter;
        critter_hits[critter_hit_count].c = g_critters[i];
        
        critter_hit_count++;

#ifndef NOVIZ_RAY_CRITTER
        viz_map_dot(r_critter.end, 10, COLOR_GREEN);
#endif
      }
    }

    // Draw each row top to bottom
    for(int row = 0; row < SCREEN_H; row++)
    {
      r_critter.pixel.y = r.pixel.y = row;

      Ray r_render = r;

      for (int i = 0; i < critter_hit_count; i++) {
        if (row > critter_hits[i].c.screen_top && row < critter_hits[i].c.screen_bot) {
          r_render = critter_hits[i].r;
          r_render.z = (float)(row - critter_hits[i].c.screen_top) / (critter_hits[i].c.screen_h);

          r_render.hit.local.y = (float)(row - critter_hits[i].c.screen_top) / (critter_hits[i].c.screen_h);
          r_render.hit.type = HIT_CRITTER;
          break;
        }
      }

      if (r_render.hit.type != HIT_CRITTER) {
        // Wall
        if (row > wall_top && row < wall_bot) {
          r_render.z = r_render.hit.local.y = (r.pixel.y - wall_top) / wall_h;
          r_render.hit.type = HIT_WALL;
        }
        // Floor/ceiling
        else {
          r_render.dist = fabs(CAMERA_HEIGHT * SCREEN_H / (r.pixel.y - SCREEN_HORIZON));
          r_render.dist = fabs(0.5 / (r.pixel.y / SCREEN_H - 0.5));


          if (row < SCREEN_HORIZON) {
            r_render.hit.type = HIT_CEIL;
            r_render.z = 1.0f;
          }
          else {
            r_render.hit.type = HIT_FLOOR;
            r_render.z = 0.0f;
          }
          r_render.end = point_add(g_player.body.pos, point_mult(r.dir, r_render.dist));
          r_render.hit.local = point_fractional(r_render.end);


  #ifndef NOVIZ_RAY_FLOOR
          viz_map_dot(r_render.end, 5, COLOR_BLUE);
  #endif
        }
      }

      r_render.pixel.y = row;
      render_ray(r_render);
    }
  }
}