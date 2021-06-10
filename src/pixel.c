#include <stdint.h>
#include <math.h>

#include "common.h"
#include "gfx.h"
#include "bitmap.h"
#include "pixel.h"
#include "viz.h"
#include "input.h"
#include "player.h"
#include "utils.h"

#define COLOR_DARKEN (Pixel)(SDL_Color) { 0, 0, 0, 255 }
#define COLOR_LIGHT (Pixel)(SDL_Color) { 255, 0, 0, 255 }
#define COLOR_CLAMP(C) (C) > 255 ? 255 : (C)

Point dither_sample;

Pixel pixel_clamp(Pixel a) {
  a.r = a.r > 255 ? 255 : a.r < 0 ? 0 : a.r;
  a.g = a.g > 255 ? 255 : a.g < 0 ? 0 : a.g;
  a.b = a.b > 255 ? 255 : a.b < 0 ? 0 : a.b;
  a.a = a.a > 255 ? 255 : a.a < 0 ? 0 : a.a;
  return a;
}

Uint8 pixel_color_add(Uint8 a, Uint8 b) {
  Uint16 sum = a + b;
  return (sum > 255) ? 255 : sum;
}

Pixel pixel_blend(Pixel a, Pixel b) {
  // if (b.a > 0) {
  //   return b;
  // }
  // else {
  //   return a;
  // }

  float alpha = b.a / 255.0;
  float alpha_inv = (1 - alpha);

  a.r = pixel_color_add((a.r * alpha_inv), (b.r * alpha));
  a.g = pixel_color_add((a.g * alpha_inv), (b.g * alpha));
  a.b = pixel_color_add((a.b * alpha_inv), (b.b * alpha));

  return a;
}



Pixel pixel_overlay(Pixel a, Pixel b) {
  // if (b.a > 0) {
  //   return b;
  // }
  // else {
  //   return a;
  // }

  Pixel out;
  float alpha_inv = (1 - b.a);

  out.r = ((a.r * alpha_inv) + (b.r * b.a));
  if (out.r < a.r) out.r = 255;
  out.g = ((a.g * alpha_inv) + (b.g * b.a));
  if (out.g < a.g) out.g = 255;
  out.b = ((a.b * alpha_inv) + (b.b * b.a));
  if (out.b < a.b) out.b = 255;

  return a;
}

Pixel pixel_lerp_linear(Pixel a, Pixel b, float t) {
  float t_inv = 1.0 - t;
  
  a.r = a.r * t_inv + b.r * t;
  a.g = a.g * t_inv + b.g * t;
  a.b = a.b * t_inv + b.b * t;
  a.a = a.a * t_inv + b.a * t;

  return a;
}

Pixel pixel_lerp_dither_bayer_4x4(Pixel a, Pixel b, float t) {
  int dither_x = dither_sample.x;
  int dither_y = dither_sample.y;

  float bayer_r = .9;
  const int bayer_n = 4;
  float bayer_matrix_4x4[][bayer_n] = {
      {    -0.5,       0,  -0.375,   0.125 },
      {    0.25,   -0.25,   0.375, - 0.125 },
      { -0.3125,  0.1875, -0.4375,  0.0625 },
      {  0.4375, -0.0625,  0.3125, -0.1875 },
  };

  Pixel color_result = b;
  float bayer_value = bayer_matrix_4x4[(int)dither_x % bayer_n][(int)dither_y % bayer_n];
  float output_color = t + (bayer_r * bayer_value);
  if (output_color < .5) {
      color_result = a;
  }
  return color_result;
}

Pixel pixel_lerp_dither_bitmap(int bitmap_idx, Pixel a, Pixel b, float t) {
  // Point dither_pt = g_gfx.screen_draw;
  
  Pixel dither_value = bitmap_sample_index(bitmap_idx, dither_sample.x, dither_sample.y, 1);
  float threshold = dither_value.r / 255.0;

  if (t > threshold) {
    return b;
  }
  return a;
}

Pixel pixel_lerp_dither_rand(Pixel a, Pixel b, float t) {
  if (rand_unit() > t) {
    return a;
  }
  return b;
}

/*
A white noise dither with the input colors moved closer to the target
result to make the random dithering less stark.
'noisiness' is the magnatude of the move towards the perfect blend.
I called it noisiness because this version of wite noise random
dithering is much softer lookiung because the colors being dithered
between are much closer together. As though they had come from a much larger
pallette if this were a bit-reduction dither. This could be worked on to
target a specific "pallette size".
But maybe not with this one becuase it was all just trial and error and 
the numbers done mean anything. For example, if one color is much closer to the 
target then should we still be jumping the same proportion of the difference towards
the target? This would make the .5 totally wong for the random dither.

Since the dithering part will only pick one of the colors, there isn't any point
in calculating both so that could be improved
*/

Pixel pixel_lerp_dither_linerand(Pixel a, Pixel b, float t) {
  float noisyness = .9;

  Pixel c = pixel_lerp_linear(a, b, t);
  Pixel d = pixel_lerp_linear(a, c, noisyness);
  Pixel e = pixel_lerp_linear(b, c, noisyness);
  return pixel_lerp_dither_rand(d, e, .5);
}

/*
Simulate an lower-bit pallet by imagining that there are a certain number
of pallet colors between our two endpoints and we are dithering between two
of those colors that are closer to the target rather than between the original
two colors. 
*/
Pixel pixel_lerp_dither_pallete_sim(Pixel a, Pixel b, float t) {
  // Number of colors to break the spread into
  int n = 2;
  float x = 1.0/n;

  float t1 = fmod(t, x);
  float ta = t - t1 / n;
  Pixel a1 = pixel_lerp_linear(a, b, ta);
  float tb = ta + n;
  Pixel b1 = pixel_lerp_linear(a1, b1, tb);

  return pixel_lerp_dither_rand(a1, b1, t1);
}

// Smooth dithering by moving the endpoints closer to the target
void pixel_lerp_smooth(Pixel *a, Pixel *b, float t, float factor) {
  Pixel a1 = pixel_lerp_linear(*a, *b, t * factor);
  Pixel b1 = pixel_lerp_linear(*b, *a, (1.0 - t) * factor);

  *a = a1;
  *b = b1;
}

Pixel pixel_lerp(Pixel a, Pixel b, float t) {
  if (t >= AAAAAAALMOST_ONE) return b;
  if (t <= SOME_TINY_AMOUNT) return a;
  
  // return pixel_lerp_linear(a, b, t);

  
  // dither_sample = point_mult(g_gfx.screen_draw, 1/16);
  dither_sample = g_gfx.screen_draw;

  if (g_input.smooth_dither) {
    pixel_lerp_smooth(&a, &b, t, 0.95);
  }

  if (g_input.dither) {
    return pixel_lerp_dither_bitmap(BITMAP_DITHER, a, b, t);
  }
  return pixel_lerp_linear(a, b, t);
}

Pixel pixel_darken(Pixel p, float t) {
  return pixel_lerp(p, COLOR_DARKEN, t);
}

Pixel pixel_light(Pixel p, Pixel c, float t) {
  // Pixel c2 = pixel_lerp_linear(p, c, 1-t);
  Pixel c2;
  c2.r = p.r * (c.r/255.0);
  c2.g = p.g * (c.g/255.0);
  c2.b = p.b * (c.b/255.0);
  
  return pixel_lerp(c2, COLOR_DARKEN, 1 - t);
}
