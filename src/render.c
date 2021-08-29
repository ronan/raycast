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

float max(float a, float b) { return (a < b) ? a : b; };
float lerp(float s, float e, float t){return s+(e-s)*t;}
float blerp(float c00, float c10, float c01, float c11, float tx, float ty){
    return lerp(lerp(c00, c10, tx), lerp(c01, c11, tx), ty);
}

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
  
  float lr, lg, lb;
  if (g_input.smooth_lights) {
    float dx = (pt.x * LIGHTMAP_RESOLUTION) - x;
    float dy = (pt.y * LIGHTMAP_RESOLUTION) - y;
    lr = blerp(g_lightmap[x][y][z][0], g_lightmap[x+1][y][z][0], g_lightmap[x][y+1][z][0], g_lightmap[x+1][y+1][z][0], dx, dy);
    lg = blerp(g_lightmap[x][y][z][1], g_lightmap[x+1][y][z][1], g_lightmap[x][y+1][z][1], g_lightmap[x+1][y+1][z][1], dx, dy);
    lb = blerp(g_lightmap[x][y][z][2], g_lightmap[x+1][y][z][2], g_lightmap[x][y+1][z][2], g_lightmap[x+1][y+1][z][2], dx, dy);
  }
  else {
    lr = g_lightmap[x][y][z][0];
    lg = g_lightmap[x][y][z][1];
    lb = g_lightmap[x][y][z][2];
  }

  // Blend the pixel color with the light color.
  c.r = fminf(255, c.r * lr);
  c.g = fminf(255, c.g * lg);
  c.b = fminf(255, c.b * lb);

  return c;
}

void render_pixel(unsigned int x, unsigned int y, Pixel color) {
  color = pixel_postprocess(x, y, color);
  gfx_put_pixel(x, y, color);
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