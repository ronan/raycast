#include "common.h"
#include "gfx.h"
#include "ray.h"
#include "wall.h"


void wall_draw(Ray *r, int col) {
  // Draw a vertical slice of the wall
  int h = (WALL_H * SCREEN_H) / (r->dist);
  int top = (SCREEN_H / 2) - (h / 2);

  for (int row = SCREEN_Y + top; row <= SCREEN_Y + top + h; row++) {
    gfx_put_pixel(SCREEN_X + col, row, WALL_COLORS[r->hit.wall]);
  }
}
