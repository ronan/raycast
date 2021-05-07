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
  // Don't draw off the map
  if (b.x < MAP_WIDTH) {
    gfx_put_line(a.x, a.y, b.x, b.y, c);
    gfx_put_square_centered(b, 2, c);
  }
}

void ray_map_draw_vector(Point a, Point b, SDL_Color c) {
  b = point_mult(b, 32);
  b = point_add(a, b);
  ray_map_draw_line(a, b, c);
}

void map_draw_line(Point a, Point b) {
  a = point_mult(a, MAP_TILES_S);
  b = point_mult(b, MAP_TILES_S);

  // Don't draw off the map
  if (b.x < MAP_WIDTH) {
    gfx_put_line(a.x, a.y, b.x, b.y, COLOR_BLACK);
    gfx_put_square_centered(b, 2, COLOR_RED);
  }
}


void ray_scan_floor_ceiling() {
  Point ray_dir_0, ray_dir_1, floor_pos, floor_step, uv;
  Point pos = point_mult(g_player.pos, 1.0/64);
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

  map_draw_line(r.pos, r.end);
  return r;
}

Ray ray_cast_step_h(Ray r) {
  r.vec = point_invert(r.vec);
  r.end = point_invert(r.end);

  r = ray_cast_step(r);

  r.vec = point_invert(r.vec);
  r.end = point_invert(r.end);
  
  map_draw_line(r.pos, r.end);
  return r;
}

Ray ray_cast(int col) {
  MapTile h_tile, v_tile;
  Ray h, v;

  Point r_origin = point_mult(g_player.pos, 1.0/MAP_TILES_S);

  float camera_x = 2.0 * col / SCREEN_W - 1.0;

  Point dir = point_add(g_player.dir, point_mult(g_player.camera_plane, camera_x));

  ray_map_draw_vector(r_origin, g_player.camera_plane, COLOR_YELLOW);
  ray_map_draw_vector(r_origin, dir, COLOR_RED);

  h = v = (Ray) {
    .pos = r_origin,
    .end = r_origin,
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
      v.dist = point_dist(v.pos, v.end);
      v.hit.wall = v.vec.x > 0 ? MAP_W : MAP_E;
      gfx_put_square_centered(point_mult(v.end, MAP_TILES_S), 1, COLOR_GREEN);
      break;
    }
  }

  for (int i = 0; i < 6; i++) {
    h = ray_cast_step_h(h);
    h.hit.wall = map_tile_at_point(h.end);
    if (map_tile_is_oob(h.hit.wall)) {
      break;
    }
    if (map_tile_is_wall(h.hit.wall)) {
      h.dist = point_dist(h.pos, h.end);
      h.hit.wall = h.vec.y > 0 ? MAP_N : MAP_S;
      gfx_put_square_centered(point_mult(h.end, MAP_TILES_S), 1, COLOR_GREEN);
      break;
    }
  }

  if (h.dist < v.dist) {
    return h;
  }
  return v;
}

void ray_scan_walls()
{
  angle r_ang, r_ang_start, r_delta;
  int step = 1;

  int r_x_min = 0;
  int r_x_max = SCREEN_W;
  
  // Draw only edge rays. For debugging
  // ray_cast(1); return;

  for (int col = r_x_min; col < r_x_max; col += step)
  {

    Ray r = ray_cast(col);

    gfx_put_square_centered(point_mult(r.end, MAP_TILES_S), 1, COLOR_BLUE);

    // Fix fisheye 
    // @TODO: This feels like it should be elsewhere.
    float x = (float)col / SCREEN_W - 0.5;
    float r_ang = atan2(x, 0.8);

    r.dist = r.dist * MAP_TILES_S;
    r.dist = r.dist * cos(r_ang);

    // Draw a vertical slice of the wall
    wall_draw(&r, col);
  }
}