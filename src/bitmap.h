#ifndef _BITMAP_H_
#define _BITMAP_H_

#include "geometry.h"
#include "pixel.h"
#include "gfx.h"

enum BITMAP_IDX {
  BITMAP_TEST,
  BITMAP_WALL,
  BITMAP_FLOOR,
  BITMAP_CEILING,
  BITMAP_BLUENOISE,
  BITMAP_BAYER,
  BITMAP_HATCH,
  BITMAP_THRESHOLD,
  BITMAP_HORIZ,
  BITMAP_DITHER,
  BITMAP_END
};

typedef SDL_Surface* BitMap;

BitMap g_bitmaps[BITMAP_END];
SDL_Texture* g_textures[BITMAP_END];

void bitmap_load_all();
void bitmap_unload_all();
Pixel bitmap_sample(int bitmap_idx, Point p);
Pixel bitmap_sample_index(int bitmap_idx, int px, int py, int wrap);
void bitmap_draw_pixel(int bitmap_idx, Point p, int dest_x, int dest_y);

#endif