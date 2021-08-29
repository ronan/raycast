#ifndef _RENDER_H_
#define _RENDER_H_

#include "bitmap.h"

Pixel render_wall(Ray r);
Pixel render_floor(Ray r);
Pixel render_ceiling(Ray r);
Pixel render_ray(Ray r);
Pixel render_critter(int type, Point sample_pt);
Pixel render_lights_at_point(Pixel c, Point3 pt);
void render_pixel(unsigned int x, unsigned int y, Pixel color);


float g_lightmap[LIGHTMAP_X][LIGHTMAP_Y][LIGHTMAP_Z][3];

#endif