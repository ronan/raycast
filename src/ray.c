#include "common.h"
#include "gfx.h"
#include "player.h"
#include "scene.h"
#include "map.h"
#include "wall.h"
#include "floor.h"
#include "geometry.h"

#include "ray.h"

void ray_map_draw_line(Point a, Point b, SDL_Color c) {
  a = point_mult(a, MINIMAP_SCALE);
  b = point_mult(b, MINIMAP_SCALE);

  // Don't draw off the map
  if (b.x < MAP_WIDTH) {
    gfx_put_line(a.x, a.y, b.x, b.y, c);
  }
}

void ray_map_draw_vector(Point a, Point b, SDL_Color c) {
  a = point_mult(a, MINIMAP_SCALE);
  b = point_mult(b, MINIMAP_SCALE);
  b = point_add(a, b);
  ray_map_draw_line(a, b, c);
}

void ray_map_draw_dot(Point a, float size, SDL_Color c) {
  a = point_mult(a, MINIMAP_SCALE);
  gfx_put_square_centered(a, size, c);
}

void ray_map_draw_ray(Ray r) {
  ray_map_draw_line(r.pos, r.end, COLOR_YELLOW);
  ray_map_draw_dot(r.end, 5, COLOR_GREEN);
}

void ray_map_draw_ray_partial(Ray r) {
  return;
  ray_map_draw_line(r.pos, r.end, COLOR_CYAN);
  ray_map_draw_dot(r.end, 5, COLOR_RED);
}


void ray_scan_floor_ceiling() {
  Point ray_dir_0, ray_dir_1, floor_pos, floor_step, uv;
  Point pos = g_player.pos;
  Point dir = g_player.dir;

  //FLOOR CASTING
  for(int y = (SCREEN_H / 2) + 1; y < SCREEN_H; y++)
  {
    // rayDir for leftmost ray (x = 0) and rightmost ray (x = w)
    ray_dir_0 = point_sub(dir, g_player.camera_plane);
    ray_dir_1 = point_add(dir, g_player.camera_plane);

    // Vertical position of the camera.
    float posZ = 0.5 * SCREEN_H;

    // Horizontal distance from the camera to the floor for the current row.
    float rowDistance = posZ / (y - posZ);

    // calculate the real world step vector we have to add for each x (parallel to camera plane)
    // adding step by step avoids multiplications with a weight in the inner loop
    floor_step = point_mult(point_sub(ray_dir_1, ray_dir_0), rowDistance);
    floor_step.x /= SCREEN_W;
    floor_step.y /= SCREEN_W;

    // real world coordinates of the leftmost column. This will be updated as we step to the right.
    Point ray = point_mult(ray_dir_0, rowDistance);
    floor_pos = point_add(pos, ray);

    ray_map_draw_vector(g_player.pos, ray_dir_0, COLOR_RED);
    ray_map_draw_vector(g_player.pos, ray_dir_1, COLOR_BLUE);
    ray_map_draw_vector(g_player.pos, g_player.dir, COLOR_BLACK);
    ray_map_draw_vector(g_player.pos, g_player.camera_plane, COLOR_YELLOW);

    for(int x = 0; x < SCREEN_W; ++x)
    {
      // gfx_put_square_centered(point_mult(floor_pos, 64), 1, COLOR_GREEN);
      //gfx_put_square_centered(floor_pos, 1, COLOR_GREEN);
      floor_draw(floor_pos, SCREEN_X + x, y);
      floor_pos = point_add(floor_pos, floor_step);
    }
  }
}

Ray ray_cast_step(Ray r) {
  Ray out = r;
  Point d;

  d.x = r.vec.x > 0 ?
    ceil(r.end.x) + 0.0001 - r.end.x:
    floor(r.end.x) - 0.0001 - r.end.x;

  d.y = d.x * (r.vec.y / r.vec.x);

  out.end = point_add(out.end, d);

  return out;
}

Ray ray_cast_step_v(Ray r) {
  r = ray_cast_step(r);
  ray_map_draw_ray_partial(r);
  return r;
}

Ray ray_invert(Ray r) {
  r.vec = point_invert(r.vec);
  r.end = point_invert(r.end);
  return r;
}

Ray ray_cast_step_h(Ray r) {
  // Horizontal steps are the same logic as vertical with the x and y inverted.
  r = ray_invert(r);
  r = ray_cast_step(r);
  r = ray_invert(r);

  ray_map_draw_ray_partial(r);
  return r;
}

Ray ray_cast(int col) {
  Ray h, v;
  float h_dist_squared = DAMN_NEAR_INFINITY, v_dist_squared = DAMN_NEAR_INFINITY;

  float camera_x = (2.0 * ((float)col / SCREEN_W)) - 1.0;

  Point dir = point_add(g_player.dir, point_mult(g_player.camera_plane, camera_x));

  ray_map_draw_vector(g_player.pos, g_player.camera_plane, COLOR_YELLOW);
  ray_map_draw_vector(g_player.pos, dir, COLOR_RED);

  h = v = (Ray) {
    .pos = g_player.pos,
    .end = g_player.pos,
    .vec = dir,
    .dist = DAMN_NEAR_INFINITY,
    .hit = RAY_HIT_OOB
  };

  for (int i = 0; i < 6; i++) {
    v = ray_cast_step_v(v);
    v.hit.tile = map_tile_at_point(v.end);
    if (map_tile_is_oob(v.hit.tile)) {
      break;
    }
    if (map_tile_is_wall(v.hit.tile)) {
      h.hit.wall = v.vec.x > 0 ? MAP_W : MAP_E;
      // The perpendicular distance avoids fisheye
      v.dist = (v.end.x - v.pos.x) / v.vec.x;
      // The euclidean distance lets us find the closest hit
      v_dist_squared = point_dist_squared(v.pos, v.end);
      break;
    }
  }

  for (int i = 0; i < 6; i++) {
    h = ray_cast_step_h(h);
    h.hit.tile = map_tile_at_point(h.end);
    if (map_tile_is_oob(h.hit.tile)) {
      break;
    }
    if (map_tile_is_wall(h.hit.tile)) {
      h.hit.wall = h.vec.y > 0 ? MAP_N : MAP_S;
      // The perpendicular distance avoids fisheye
      h.dist = (h.end.y - h.pos.y) / h.vec.y;
      // The euclidean distance lets us find the closest hit
      h_dist_squared = point_dist_squared(h.pos, h.end);
      break;
    }
  }

  Ray r = h_dist_squared < v_dist_squared ? h : v;
  
  ray_map_draw_ray(r);
  wall_draw(&r, col);

  return r;
}

void ray_scan_walls()
{
  // Draw only 3 rays. For debugging
  // ray_cast(SCREEN_W/2); return;
  // ray_cast(1); ray_cast(SCREEN_W/2); ray_cast(SCREEN_W); return;

  for (int col = 0; col < SCREEN_W; col += 1)
  {
    ray_cast(col);
  }
}