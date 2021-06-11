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
  if (!RENDER_LIGHTS || c.a == 0) {
    return c;
  }

  // Ambient
  float brightness = 0.2;
  Pixel light_color = (SDL_Color) {255, 255, 255, 255};

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
      // If the dropoff is to stark add a 'b' term (ie: b * sqrt(dist_sq)))
      float att = clamp(1 - ((dist_sq * 1)));

      brightness += att*g_critters[i].glow;
      light_color = pixel_lerp_linear(light_color, g_critters[i].glow_color, att*g_critters[i].glow);
    }
  }
  return pixel_light(c, light_color, brightness);
}

// Render a ray
Pixel render_ray(Ray r) {
  Pixel p = COLOR_CLEAR;

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
      switch (r.hit.critter->type) {
        case CRITTER_LIGHT:
          p = bitmap_sample(BITMAP_LIGHT, r.hit.local);
        break;
        case CRITTER_ORB:
          p = bitmap_sample(BITMAP_CRITTER, r.hit.local);
        break;
        default:
          p = COLOR_BLACK;
        break;
      }
    break;
    default:
      p = COLOR_BLACK;
    break;
  }
  p = render_lights(r, p);

  return p;
}
