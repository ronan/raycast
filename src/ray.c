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

float g_lightmap[LIGHTMAP_X][LIGHTMAP_Y][LIGHTMAP_Z][3];

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


Point ray_cast_step(Point p, Point dir) {
  Point d;

  d.x = dir.x > 0 ?
    ceil(p.x) + 0.0001 - p.x:
    floor(p.x) - 0.0001 - p.x;

  d.y = d.x * (p.y / p.x);
  return point_add(p, d);
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
  Pixel c;
  float z_buffer[SCREEN_W][SCREEN_H];
  // float d;
  double ignore;
  Point sample_pt;
  
  Point ray_from = point_sub(g_camera_dir, point_mult(g_camera_plane, FOV));
  Point ray_to = point_sub(g_camera_dir, point_mult(g_camera_plane, -FOV));
  Point ray_delta = point_mult(point_sub(ray_to, ray_from), 1.0/SCREEN_W);

  for(int row = 0; row < SCREEN_H; row++)
    for (int col = 0; col < SCREEN_W; col++)
      z_buffer[col][row] = DAMN_NEAR_INFINITY;

  for (int x = 0; x < LIGHTMAP_X; x++) {
    for (int y = 0; y < LIGHTMAP_Y; y++) {
      for (int z = 0; z < LIGHTMAP_Z; z++) {
        g_lightmap[x][y][z][0] = 0.1;
        g_lightmap[x][y][z][1] = 0.1;
        g_lightmap[x][y][z][2] = 0.1;
      }
    }
  }

  if (RENDER_LIGHTS) {
    for (int i = 0; i < MAX_CRITTERS; i++) {
      if (g_critters[i].glow < SOME_TINY_AMOUNT) continue;

      float r = (g_critters[i].glow_color.r/255.0f) * g_critters[i].glow;
      float g = (g_critters[i].glow_color.g/255.0f) * g_critters[i].glow;
      float b = (g_critters[i].glow_color.b/255.0f) * g_critters[i].glow;

      int lx = (u_int8_t)(g_critters[i].body.pos.x * LIGHTMAP_RESOLUTION);
      int ly = (u_int8_t)(g_critters[i].body.pos.y * LIGHTMAP_RESOLUTION);
      int lz = (u_int8_t)(g_critters[i].body.z * LIGHTMAP_RESOLUTION);

      int x_frm = fmaxf(0, lx - LIGHTMAP_MAX_LIGHT_RADIUS);
      int x_to = fminf(LIGHTMAP_X, lx + LIGHTMAP_MAX_LIGHT_RADIUS);
        
      int y_frm = fmaxf(0, ly - LIGHTMAP_MAX_LIGHT_RADIUS);
      int y_to = fminf(LIGHTMAP_Y, ly + LIGHTMAP_MAX_LIGHT_RADIUS);

      int z_frm = fmaxf(0, lz - LIGHTMAP_MAX_LIGHT_RADIUS);
      int z_to = fminf(LIGHTMAP_Z, lz + LIGHTMAP_MAX_LIGHT_RADIUS);

      for (int x = x_frm; x < x_to; x++) {
        float dx = (float)(lx - x) / LIGHTMAP_RESOLUTION;
        for (int y = y_frm; y < y_to; y++) {         
          // Does the ray have line of sight with the source.
          Point end = (Point){(float)x / LIGHTMAP_RESOLUTION, (float)y / LIGHTMAP_RESOLUTION};
          Point start = g_critters[i].body.pos;
          if ((int)start.x != (int)end.x && (int)start.y != (int)end.y) {
            float mapx, mapy;
            Point dir = point_sub(end, start);

            // Find where the ray exits the source tile horizontally
            mapx = end.x > start.x ? ceil(start.x) + 0.0001 : floor(start.x) - 0.0001;
            mapy = start.y + (mapx - start.x) * (dir.y / dir.x);
            if(g_map[(int)mapy][(int)mapx] == MAP_TILE_WALL) continue;

            // Find where the ray exits the source tile vertically
            mapy = end.y > start.y ? ceil(start.y) + 0.0001 : floor(start.y) - 0.0001;
            mapx = start.x + (mapy - start.y) * (dir.x / dir.y);
            if(g_map[(int)mapy][(int)mapx] == MAP_TILE_WALL) continue;
          }
          
          float dy = (float)(ly - y) / LIGHTMAP_RESOLUTION;
          for (int z = z_frm; z < z_to; z++) {
            float dz = (float)(lz - z) / LIGHTMAP_RESOLUTION;
            float dist2 = (dx * dx) + (dy * dy) + (dz * dz);
            float att = 1 / (1 + (dist2 * dist2 * dist2));

            g_lightmap[x][y][z][0] += r * att;
            g_lightmap[x][y][z][1] += g * att;
            g_lightmap[x][y][z][2] += b * att;
          }
        }
      }
    }
  }


  float d = DAMN_NEAR_INFINITY;
  if (RENDER_WALLS) {
    Point ray = ray_from;
    for (int col = 0; col < SCREEN_W; col++)
    {
      r = create_ray();
      g_gfx.screen_draw.x = r.pixel.x = col;

      r.dir = ray;
      ray = point_add(ray, ray_delta);
      r.start = r.end = g_player.body.pos;

      float closest_d_sq = DAMN_NEAR_INFINITY;
      Ray r_h, r_v = r;
      for (int i = 0; i < MAP_TILES_X * 2; i++) {
        r_h.end = ray_cast_step_point(r);
        r_v.end = ray_cast_step_point_inv(r);

        float d_sq_h = point_dist_squared(r.start, r_h.end);
        float d_sq_v = point_dist_squared(r.start, r_v.end);

        r.end = d_sq_h < d_sq_v ? r_h.end : r_v.end;
        r.hit.tile = map_tile_at_point(r.end);
        if (map_tile_is_wall(r.hit.tile)) {
          if (d_sq_h < d_sq_v) {
            // The closest wall or critter at this column
            closest_d_sq = d_sq_h;
            // The perpendicular distance avoids fisheye
            d = (r.end.x - r.start.x) / r.dir.x;
            // Local wall coordinates of hit
            sample_pt.x = (point_fractional(r.end)).y;
          }
          else {
            closest_d_sq = d_sq_v;
            // The perpendicular distance avoids fisheye
            d = (r.end.y - r.start.y) / r.dir.y;
            // Local wall coordinates of hit
            sample_pt.x = (point_fractional(r.end)).x;
          }
          break;
        }
      }

      int wall_h = SCREEN_H / d;
      int wall_top = SCREEN_HORIZON - (wall_h / 2.0);
      int wall_bottom = wall_top + wall_h;

      // Draw each row top to bottom
      for(int row = 0; row < SCREEN_H; row++)
      {
        g_gfx.screen_draw = (Point){ col, row };

        // Wall
        if (row > wall_top && row < wall_bottom) {
          z_buffer[col][row] = d;
  
          sample_pt.y = (float)(row - wall_top) / wall_h;
          
          c = bitmap_sample(BITMAP_WALL, sample_pt);
          c = render_lights_at_point(c, (Point3){r.end.x, r.end.y, 1 - sample_pt.y});
          render_pixel(col, row, c);
        }
      }
    }
  }

  if (RENDER_CEILING_FLOOR) {
    d = DAMN_NEAR_INFINITY;
    for(int row = 0; row < SCREEN_H / 2; row++) {
      d = fabs(CAMERA_HEIGHT * SCREEN_H / (row - SCREEN_HORIZON));
      Point ray = ray_from;
      for (int col = 0; col < SCREEN_W; col++) {
        ray = point_add(ray, ray_delta);
        if (z_buffer[col][row] < d) continue;
        
        z_buffer[col][row] = d;
        g_gfx.screen_draw = (Point){col, row};

        Point p = point_add(g_camera_pos, point_mult(ray, d));

        c = bitmap_sample(BITMAP_CEILING, p);
        c = COLOR_CEILING;
        c = render_lights_at_point(c, (Point3){p.x, p.y, 1.0});
        render_pixel(col, row, c);

        c = bitmap_sample(BITMAP_FLOOR, p);
        c = COLOR_FLOOR;
        c = render_lights_at_point(c, (Point3){p.x, p.y, 0});
        render_pixel(col, SCREEN_H - 1 - row, c);
      }
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
      float dist = point_dist(g_critters[i].body.pos, par);

      // Item is in front of the camera
      if (dist > 0 && point_dot(pos_diff, g_camera_dir) > 0) {
        float width = (((g_critters[i].body.radius * 2.0) / dist)) * SCREEN_H;
        float height = (((g_critters[i].body.height) / dist)) * SCREEN_H;

        // Camera space calculation
        float transform_x = inv_det * point_cross(pos_diff, g_camera_dir);
        float transform_y = inv_det * point_cross(pos_diff, cam_plane);

        int screen_pos_x = (SCREEN_W/2) - (width/2) - (transform_x / transform_y) * FOV * SCREEN_W;
        int screen_pos_y = (SCREEN_H/2) + (SCREEN_H/dist/2) - ((g_critters[i].body.z * SCREEN_H / dist) + (height));

        for (int x = 0; x < width; x++) {
          int px_x = screen_pos_x + x;
          if (px_x > 0 && px_x < SCREEN_W) {
            for (int y = 0; y < height; y++) {
              int px_y = screen_pos_y + y;
              if (px_y > 0 && px_y < SCREEN_H) {
                if (z_buffer[px_x][px_y] >= dist) {
                  g_gfx.screen_draw = (Point){px_x, px_y};
                  Pixel p = render_critter(g_critters[i].type, (Point){x/width, y/height});
                  p = render_lights_at_point(p, (Point3){g_critters[i].body.pos.x, g_critters[i].body.pos.y, g_critters[i].body.z});
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

          int screen_radius_sq = radius * radius * SCREEN_H * SCREEN_H;

          for (int x = fr_x; x < to_x; x++) {
            for (int y = fr_y; y < to_y; y++) {
              if (z_buffer[x][y] >= d) {
                if (point_dist_squared((Point){x, y}, (Point){screen_pos_x, screen_pos_y}) < screen_radius_sq) {
                  gfx_overlay_pixel(x, y, g_particles[i].color);
                }
              }
            }
          }
        }
      }
    }
  }
}
