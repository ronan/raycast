#include <stdint.h>
#include <math.h>

#include "common.h"
#include "gfx.h"
#include "bitmap.h"
#include "pixel.h"
#include "viz.h"
#include "input.h"

#define COLOR_DARKEN (Pixel)(SDL_Color) { 0, 0, 0, 0 }

Point dither_sample;

Pixel pixel_lerp_linear(Pixel a, Pixel b, float t) {
  float t_inv = 1.0 - t;
  
  a.r = a.r * t_inv + b.r * t;
  a.g = a.g * t_inv + b.g * t;
  a.b = a.b * t_inv + b.b * t;

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

Pixel pixel_lerp_dither_bitmap(Pixel a, Pixel b, float t) {
  Point dither_pt = g_gfx.screen_draw;
  int dx = (int)(dither_pt.x) % 16;
  int dy = (int)(dither_pt.y) % 16;

  // float threshold = .5;
  
  Pixel dither_value = bitmap_sample_index(BITMAP_BAYER, dx, dy);
  float threshold = dither_value.r / 255.0;

  if (t > threshold) {
    return b;
  }
  return a;
}

Pixel pixel_lerp_dither_rand(Pixel a, Pixel b, float t) {
  if ((float)rand() / RAND_MAX > t) {
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


Pixel pixel_lerp(Pixel a, Pixel b, float t) {
  if (t >= .999) return b;
  if (t <= 0.001) return a;


  dither_sample = g_gfx.screen_draw;

  if (g_input.dither) {
    return pixel_lerp_dither_bitmap(a, b, t);
  }
  return pixel_lerp_linear(a, b, t);
}

Pixel pixel_darken(Pixel p, float t) {
  return pixel_lerp(p, COLOR_DARKEN, t);
}
