#include "common.h"
#include "geometry.h"
#include "bitmap.h"
#include "pixel.h"
#include "scene.h"
#include "map.h"
#include "ray.h"

#include "render.h"

Pixel render_floor(Ray r, int screen_x, int screen_y) {
  // Get the fractional part of the position to determine the texture coordinates
  Pixel p = bitmap_sample(BITMAP_FLOOR, r.hit.local);
  p = pixel_darken(p, 1.0 - 1.0/r.dist);
  return p;
}

Pixel render_ceiling(Ray r, int screen_x, int screen_y) {
  Pixel p = bitmap_sample(BITMAP_CEILING, r.hit.local);
  p = pixel_darken(p, 1.0 - 1.0/r.dist);
  return p;
}

// pos_local is the position in coordinates on the wall (x, z) or (y, z). Range 0.0-1.0
Pixel render_wall(Ray r, int screen_x, int screen_y) {
  Pixel p = bitmap_sample(BITMAP_WALL, r.hit.local);
  
  // srand(screen_x * SCREEN_W + screen_y);
  p = pixel_darken(p, 1.0 - 1.0/r.dist);

  return p;
}
