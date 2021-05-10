#ifndef _RENDER_H_
#define _RENDER_H_

#include "bitmap.h"

Pixel render_wall(Ray r, int screen_x, int screen_y);
Pixel render_floor(Ray r, int screen_x, int screen_y);
Pixel render_ceiling(Ray r, int screen_x, int screen_y);

#endif