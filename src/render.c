#include "common.h"
#include "geometry.h"
#include "bitmap.h"
#include "pixel.h"
#include "scene.h"
#include "map.h"
#include "gfx.h"
#include "input.h"
#include "critter.h"
#include "viz.h"
#include "ray.h"

#include "render.h"

// Pixel g_lightmap[LIGHTMAP_W][LIGHTMAP_H];

float clamp(float f) {
  if (f > AAAAAAALMOST_ONE) return 1.0;
  if (f < SOME_TINY_AMOUNT) return 0.0;
  return f;
}

Pixel render_lights_at_point(Pixel c, Point3 pt) {
  if (!RENDER_LIGHTS || g_input.lights_off || c.a == 0) {
    return c;
  }

  // TODO: Make light falloff more realistic
  // TODO: Blow out extremely bright areas by blending to pure light color

  // Ambient
  Pixel l = (SDL_Color) {51, 51, 51, 0};
  float r = 0.2, g = 0.2, b = 0.2;

  for (int i = 0; i < MAX_CRITTERS; i++) {
    if (g_critters[i].glow < SOME_TINY_AMOUNT) continue;

    float x = g_critters[i].body.pos.x - pt.x;
    float y = g_critters[i].body.pos.y - pt.y;
    float z = g_critters[i].body.z - pt.z;
    float dist_sq = (x * x) + (y * y) + (z * z);

    // Limit the effect light can have.
    if (dist_sq > 1) continue;

    // The light's brightness times the inverse of it's distance
    float t = (1 - dist_sq) * g_critters[i].glow;
    
    l.r = l.r + g_critters[i].glow_color.r * t;
    l.g = l.g + g_critters[i].glow_color.g * t;
    l.b = l.b + g_critters[i].glow_color.b * t;
  }

  // Blend the pixel color with the light color.
  c.r = c.r * (l.r/255.0);
  c.g = c.g * (l.g/255.0);
  c.b = c.b * (l.b/255.0);

  return c;
}

// Pixel render_lights_from_map(Pixel c, Point3 pt) {
//   if (!RENDER_LIGHTS || g_input.lights_off || c.a == 0) {
//     return c;
//   }

//   int x = (u_int8_t)(pt.x * LIGHTMAP_RESOLUTION);
//   int y = (u_int8_t)(pt.y * LIGHTMAP_RESOLUTION);
  
//   Pixel l = g_lightmap[x][y];

//   // Blend the pixel color with the light color.
//   c.r = c.r * (l.r/255.0);
//   c.g = c.g * (l.g/255.0);
//   c.b = c.b * (l.b/255.0);

//   return c;
// }


Pixel render_critter(int type, Point sample_pt) {
  Pixel p = COLOR_CLEAR;
  switch (type) {
    case CRITTER_LIGHT:
      p = bitmap_sample(BITMAP_LIGHT, sample_pt);
    break;
    case CRITTER_ORB:
      p = bitmap_sample(BITMAP_CRITTER, sample_pt);
    break;
    default:
      p = COLOR_BLACK;
    break;
  }
  return p;
}