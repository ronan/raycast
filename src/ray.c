#include "common.h"
#include "gfx.h"
#include "player.h"
#include "scene.h"
#include "map.h"
#include "wall.h"
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

void ray_draw_all()
{
  angle r_ang, r_ang_start, r_delta;
  int step = 1;

  int r_x_min = 0;
  int r_x_max = SCREEN_W;
  
  // Draw only edge rays. For debugging
  ray_cast(1);
  // ray_cast(g_player.pos, g_player.ang, SCREEN_W / 2);
  // ray_cast(g_player.pos, g_player.ang, SCREEN_W);
  // r_x_max = 0;

  for (int col = r_x_min; col < r_x_max; col += step)
  {
    float x = (float)col / SCREEN_W - 0.5;
    float r_ang = atan2(x, 0.8);

    Ray r = ray_cast(col);

    gfx_put_square_centered(point_mult(r.end, MAP_TILES_S), 1, COLOR_BLUE);

    // Fix fisheye
    r.dist = r.dist * MAP_TILES_S;
    r.dist = r.dist * cos(r_ang);

    // Draw a vertical slice of the wall
    wall_draw(&r, col);
  }
}