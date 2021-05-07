#include "common.h"
#include "gfx.h"
#include "ray.h"
#include "wall.h"
#include "bitmap.h"

Point wall_local_hit_point_from_ray(Ray *r) {
  Point local, out = (Point){0,0};
  local.x = r->end.x - floor(r->end.x);
  local.y = r->end.y - floor(r->end.y);

  if (r->hit.wall == MAP_S || r->hit.wall == MAP_N) {
    out.x = local.x;
  }
  if (r->hit.wall == MAP_E || r->hit.wall == MAP_W) {
    out.x = local.y;
  }

  // if (r->hit.wall == MAP_S || r->hit.wall == MAP_W) {
  //   out.x = 1.0 - out.x;
  // }
  return out;
}

void wall_draw(Ray *r, int col) {
  // Draw a vertical slice of the wall
  int h = (WALL_H * SCREEN_H) / (r->dist);
  int top = (SCREEN_H / 2) - (h / 2);
  Point uv = wall_local_hit_point_from_ray(r);
  Pixel p;

  float uv_delta_y = 1.0 / h;
  for (int row = SCREEN_Y + top; row <= SCREEN_Y + top + h; row++) {
    bitmap_draw_pixel(BITMAP_WALL, uv, SCREEN_X + col, row);
    uv.y += uv_delta_y;
  }
}
