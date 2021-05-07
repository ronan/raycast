#include "common.h"
#include "geometry.h"
#include "player.h"
#include "floor.h"
#include "bitmap.h"
#include "ray.h"


void floor_draw(Point pos, int dest_x, int dest_y) {
  // Get the fractional part of the position to determine the texture coordinates
  pos = point_fractional(pos);
  bitmap_draw_pixel(BITMAP_FLOOR, pos, dest_x, dest_y);
}