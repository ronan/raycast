#ifndef _RENDER_H_
#define _RENDER_H_

#include "bitmap.h"

Pixel render_wall(Ray r);
Pixel render_floor(Ray r);
Pixel render_ceiling(Ray r);
Pixel render_ray(Ray r);
Pixel render_critter(int type, Point sample_pt);
Pixel render_lights_at_point(Pixel c, Point3 pt);

#endif