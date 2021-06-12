#ifndef _RENDER_H_
#define _RENDER_H_

#include "bitmap.h"

Pixel render_wall(Ray r);
Pixel render_floor(Ray r);
Pixel render_ceiling(Ray r);
Pixel render_ray(Ray r);
Pixel render_critter(int type, Point sample_pt);

#endif