#ifndef _SCN_H_
#define _SCN_H_

#include "gfx.h"

gfx_err scn_draw();

#define COLOR_BG \
  (SDL_Color) { 55, 55, 55, 0 }
#define COLOR_TILE \
  (SDL_Color) { 10, 10, 10, 0 }
#define COLOR_CEILING \
  (SDL_Color) { 10, 10, 100, 0 }
#define COLOR_FLOOR \
  (SDL_Color) { 10, 100, 10, 0 }
#define COLOR_WALL \
  (SDL_Color) { 200, 200, 200, 0 }
#define COLOR_WALL_LIT \
  (SDL_Color) { 240, 240, 240, 0 }

#endif