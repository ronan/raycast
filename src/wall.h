#ifndef _WALL_H_
#define _WALL_H_


#include "geometry.h"
#include "ray.h"
#include "map.h"

#define WALL_H MAP_TILES_S


void wall_draw(Ray *r, int col);

#endif