#ifndef _BITMAP_H_
#define _BITMAP_H_

#include "geometry.h"
#include "gfx.h"

enum BITMAP_IDX {
  BITMAP_TEST,
  BITMAP_WALL,
  BITMAP_FLOOR,
  BITMAP_END
};

typedef SDL_Surface* BitMap;
typedef SDL_Color* Pixel;

BitMap g_bitmaps[BITMAP_END];

void bitmap_load_all();
void bitmap_unload_all();
Pixel bitmap_sample(int bitmap_idx, Point p);

#endif