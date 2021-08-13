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

  int x = (u_int8_t)(pt.x * LIGHTMAP_RESOLUTION);
  int y = (u_int8_t)(pt.y * LIGHTMAP_RESOLUTION);
  int z = (u_int8_t)(pt.z * LIGHTMAP_Z);
  
  // Blend the pixel color with the light color.
  c.r = fminf(255, c.r * g_lightmap[x][y][z][0]);
  c.g = fminf(255, c.g * g_lightmap[x][y][z][1]);
  c.b = fminf(255, c.b * g_lightmap[x][y][z][2]);

  return c;
}


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