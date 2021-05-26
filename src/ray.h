#ifndef _RAY_H_
#define _RAY_H_

#include "map.h"

#define DAMN_NEAR_INFINITY 10000
#define RAY_HIT_OOB (RayHit) {.tile = MAP_TILE_OOB, .local = POINT_ORIGIN}
#define RAY_OOB (Ray) {.pos = POINT_OOB, .len = DAMN_NEAR_INFINITY, .hit = RAY_HIT_OOB};

typedef struct RayHit {
  MapTile tile;
  MapDir wall;
  Point local;
} RayHit;

typedef struct Ray {
  Point start;
  Point end;
  Point dir;
  Point pixel;
  angle ang;
  float dist;
  RayHit hit;
} Ray;

int ray_billboard_intersection(Ray *r, Point c, float radius);
void ray_floor_ceiling_scan();
void ray_scan_walls();

#endif