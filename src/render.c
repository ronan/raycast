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

float clamp(float f) {
  if (f > AAAAAAALMOST_ONE) return 1.0;
  if (f < SOME_TINY_AMOUNT) return 0.0;
  return f;
}

Pixel render_lights(Ray r, Pixel c) {

  // Ambient
  float brightness = 0.1;

  // TODO: Prorperly calcualte light z-effect
  // TODO: Make light falloff more realistic
  // TODO: Blow out extremely bright areas by blending to pure light color

  float intensity = .02;
  float a = 1;
  float b = .05;

  for (int i = 0; i < MAX_CRITTERS; i++) {
    if (g_critters[i].glow > SOME_TINY_AMOUNT) {
      float x = g_critters[i].body.pos.x - r.end.x;
      float y = g_critters[i].body.pos.y - r.end.y;
      float z = g_critters[i].body.z - r.z;

      float dist_sq = (x * x) + (y * y) + (z * z);
      float att = clamp(1 - ((dist_sq * 1) + (sqrt(dist_sq) * .1)));

      brightness += att*g_critters[i].glow;
    }
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
    default:
      p = COLOR_BLACK;
    break;
  }
  p = render_lights(r, p);

  gfx_put_pixel(r.pixel.x, r.pixel.y, p);
}
