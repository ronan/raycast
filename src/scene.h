#ifndef _SCN_H_
#define _SCN_H_

#include "gfx.h"

void scn_draw();

#define COLOR_BG \
  (SDL_Color) { 55, 55, 55, 0 }
#define COLOR_TILE \
  (SDL_Color) { 10, 10, 10, 0 }
#define COLOR_CEILING \
  (SDL_Color) { 70, 80, 170, 255 }
#define COLOR_FLOOR \
  (SDL_Color) { 10, 100, 10, 255 }
#define COLOR_WALL \
  (SDL_Color) { 200, 200, 200, 255 }
#define COLOR_WALL_LIT \
  (SDL_Color) { 240, 240, 240, 0 }

#endif