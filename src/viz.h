#ifndef _VIZ_H_
#define _VIZ_H_

#include "geometry.h"
#include "ray.h"

#define VIZ_MAP_SCALE 64
#define VIZ_MAP_W 512
#define VIZ_MAP_H 512
#define VIZ_WINDOW_W 512
#define VIZ_WINDOW_H 512

gfx_err viz_init(unsigned int width, unsigned int height);
void viz_destroy();

gfx_err viz_update();

void viz_map();

void viz_map_line(Point a, Point b, SDL_Color c);
void viz_map_vector(Point a, Point b, SDL_Color c);
void viz_map_dot(Point a, float size, SDL_Color c);
void viz_map_ray(Ray r);
void viz_map_ray_partial(Ray r);



#endif