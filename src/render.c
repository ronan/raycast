#include "common.h"
#include "geometry.h"
#include "bitmap.h"
#include "pixel.h"
#include "scene.h"
#include "map.h"
#include "gfx.h"
#include "critter.h"
#include "viz.h"
#include "ray.h"

#include "render.h"

Pixel render_lights(Ray r, Pixel c) {
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

// Render a ray
void render_ray(Ray r) {
  Pixel p;

  // Clamp the local sample point to account for floating point errors
  r.hit.local.y = r.hit.local.y >= 1.0 ? 0.99 : r.hit.local.y;
  r.hit.local.y = r.hit.local.y <= 0.0 ? 0.0  : r.hit.local.y;

  switch (r.hit.type) {
    case HIT_CEIL:
      p = bitmap_sample(BITMAP_CEILING, r.hit.local);
    break;
    case HIT_FLOOR:
      p = bitmap_sample(BITMAP_FLOOR, r.hit.local);
    break;
    case HIT_WALL:
      p = bitmap_sample(BITMAP_WALL, r.hit.local);
    break;    
    case HIT_CRITTER:
      p = bitmap_sample(BITMAP_CRITTER, r.hit.local);
    break;
    case HIT_NONE:
      p = COLOR_CLEAR;
    break;
  }

  p = render_lights(r, p);

  gfx_put_pixel(r.pixel.x, r.pixel.y, p);
}
