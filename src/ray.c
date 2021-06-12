#include <math.h>

#include "common.h"
#include "utils.h"
#include "gfx.h"
#include "player.h"
#include "scene.h"
#include "map.h"
#include "geometry.h"
#include "viz.h"
#include "render.h"
#include "pixel.h"
#include "critter.h"
#include "particle.h"

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
  Pixel p;
  float z_buffer[SCREEN_W][SCREEN_H];

  for (int col = 0; col < SCREEN_W; col++)
  {
    r = create_ray();
    g_gfx.screen_draw.x = r.pixel.x = col;
    r.hit.type = HIT_WALL;

    // Set the direction of the ray based on the camera properties and current column
    float camera_x = (2.0 * (r.pixel.x / SCREEN_W)) - 1.0;
    float camera_plane_length = FOV * camera_x;
    Point camera_plane = point_mult(g_camera_plane, camera_plane_length);

    r.dir = point_add(g_player.body.dir, camera_plane);
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
          closest_d_sq = d_sq_h;
          // The perpendicular distance avoids fisheye
          r.dist = (r.end.x - r.start.x) / r.dir.x;
          // Local wall coordinates of hit
          r.hit.local.x = (point_fractional(r.end)).y;
        }
        else {
          closest_d_sq = d_sq_v;
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
    
    // Draw each row top to bottom
    for(int row = 0; row < SCREEN_H; row++)
    {
      g_gfx.screen_draw.y = r.pixel.y = row;
      Ray r_render = r;
      p = COLOR_BLACK;

      // Wall
      if (row > wall_top && row < wall_bot) {
        r_render.hit.local.y = (r.pixel.y - wall_top) / wall_h;
        r_render.z = 1 - r_render.hit.local.y;
        r_render.hit.type = HIT_WALL;


        z_buffer[col][row] = wall_dist;

        p = render_ray(r_render);
      }
      // Floor/ceiling
      else 
      {
        r_render.dist = fabs(CAMERA_HEIGHT * SCREEN_H / (r.pixel.y - SCREEN_HORIZON));
        r_render.dist = fabs(0.5 / (r.pixel.y / SCREEN_H - 0.5));


        z_buffer[col][row] = r_render.dist;


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
        
        p = render_ray(r_render);

#ifndef NOVIZ_RAY_FLOOR
        viz_map_dot(r_render.end, 5, COLOR_BLUE);
#endif
      }

      gfx_put_pixel(r.pixel.x, r.pixel.y, p);
    }
  }

  float camera_plane_length = FOV * SCREEN_RATIO;
  Point cam_plane = point_mult(g_camera_plane, camera_plane_length);

  double inv_det = 1.0 / point_cross(cam_plane, g_camera_dir);

  if (RENDER_CRITTERS) {
    for (int i = 0; i < MAX_CRITTERS; i++) {
      Point pos_diff = point_sub(g_critters[i].body.pos, g_camera_pos);

      // Get the paralel distance by projecting the ray from the camera to the particle onto the camera plane.
      float t = point_dot(pos_diff, g_camera_plane);
      Point par = point_add(g_camera_pos, point_mult(g_camera_plane, t));
      float d = point_dist(g_critters[i].body.pos, par);

      // Particle is in front of the camera
      if (point_dot(pos_diff, g_camera_dir) > 0) {
        float width = (((g_critters[i].body.radius * 2) / d)) * SCREEN_H;
        float height = (((g_critters[i].body.height) / d)) * SCREEN_H;

        // Camera space calculation
        double transform_x = inv_det * point_cross(pos_diff, g_camera_dir);
        double transform_y = inv_det * point_cross(pos_diff, cam_plane);

        int screen_pos_x = (SCREEN_W/2) - (width/2) - (transform_x / transform_y) * FOV * SCREEN_W;
        int screen_pos_y = (SCREEN_H/2) + (SCREEN_H/d/2) - ((g_critters[i].body.z * SCREEN_H / d) + (height));

        for (int x = 0; x < width; x++) {
          int px_x = screen_pos_x + x;
          if (px_x > 0 && px_x < SCREEN_W) {
            for (int y = 0; y < height; y++) {
              int px_y = screen_pos_y + y;
              if (px_y > 0 && px_y < SCREEN_H) {
                if (z_buffer[px_x][px_y] >= d) {
                  Point sample_pt = (Point){x/width, y/height};
                  Pixel p = render_critter(g_critters[i].type, sample_pt);
                  gfx_overlay_pixel(px_x, px_y, p);
                }
              }
            }
          }
        }
      }
    }
  }

  // Particles
  if (RENDER_PARTICLES) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
      if (particle_is_alive(&g_particles[i])) {
        Point pos = point3_to_point(g_particles[i].pos);
        Point particle_diff = point_sub(pos, g_camera_pos);

        // Get the paralel distance by projecting the ray from the camera to the particle onto the camera plane.
        float t = point_dot(particle_diff, g_camera_plane);
        Point par = point_add(g_camera_pos, point_mult(g_camera_plane, t));
        float d = point_dist(pos, par);

        // Particle is in front of the camera
        if (point_dot(particle_diff, g_camera_dir) > 0) {
          // Camera space calculation
          float camera_plane_length = FOV * SCREEN_RATIO;
          Point cam_plane = point_mult(g_camera_plane, camera_plane_length);
          float radius = (((g_particles[i].radius) / d) / 2);

          double inv_det = 1.0 / point_cross(cam_plane, g_camera_dir);
          double transform_x = inv_det * point_cross(particle_diff, g_camera_dir);
          double transform_y = inv_det * point_cross(particle_diff, cam_plane);

          int screen_pos_x = (SCREEN_W/2) - (transform_x / transform_y) * FOV * SCREEN_W;
          int screen_pos_y = (SCREEN_H/2) + (SCREEN_H/d/2) - (g_particles[i].pos.z * SCREEN_H / d);
          int fr_x = clamp_int(screen_pos_x - radius / SCREEN_RATIO * SCREEN_W, 0, SCREEN_W);
          int to_x = clamp_int(screen_pos_x + radius / SCREEN_RATIO * SCREEN_W, 0, SCREEN_W);
          int fr_y = clamp_int(screen_pos_y - radius * SCREEN_H, 0, SCREEN_H);
          int to_y = clamp_int(screen_pos_y + radius * SCREEN_H, 0, SCREEN_H);

          for (int x = fr_x; x < to_x; x++) {
            for (int y = fr_y; y < to_y; y++) {
              if (z_buffer[x][y] >= d) {
              // if (point_dist_squared(px, particle_camera) < particle_camera_radius_sq) {
                gfx_overlay_pixel(x, y, g_particles[i].color);
              // }
              }
            }
          }
        }
      }
    }
  }
}