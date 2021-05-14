#include "common.h"
#include "geometry.h"
#include "bitmap.h"
#include "pixel.h"
#include "scene.h"
#include "map.h"
#include "gfx.h"
#include "ray.h"

#include "render.h"


Pixel render_light(Ray r, Pixel c) {
  float intensity = .1;

  // Ambient
  float brightness = .1;

  for (int i = 0; i < 16; i++) {
    Point diff = point_sub(r.end, g_lights[i]);
    float dist = diff.x * diff.x + diff.y * diff.y;

    brightness += (1.0/dist)*intensity;
  }

  if (brightness > .999) {
    return c;
  }
  return pixel_darken(c, 1.0 - brightness);
}

Pixel render_floor(Ray r) {
  // Get the fractional part of the position to determine the texture coordinates
  Pixel p = bitmap_sample(BITMAP_FLOOR, r.hit.local);
  p = render_light(r, p);
  return p;
}

Pixel render_ceiling(Ray r) {
  Pixel p = bitmap_sample(BITMAP_CEILING, r.hit.local);
  p = render_light(r, p);
  return p;
}

// pos_local is the position in coordinates on the wall (x, z) or (y, z). Range 0.0-1.0
Pixel render_wall(Ray r) {
  Pixel p = bitmap_sample(BITMAP_WALL, r.hit.local);
  
  p = render_light(r, p);

  return p;
}
