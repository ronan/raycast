#ifndef _RAY_H_
#define _RAY_H_

#include "map.h"

#define DAMN_NEAR_INFINITY 10000
#define RAY_HIT_OOB (RayHit) {.tile = MAP_TILE_OOB}
#define RAY_OOB (Ray) {.pos = POINT_OOB, .len = DAMN_NEAR_INFINITY, .hit = RAY_HIT_OOB};

typedef struct RayHit {
  MapTile tile;
  MapDir wall;
  Point pt;
} RayHit;

typedef struct Ray {
  Point pos;
  Point end;
  Point vec;
  angle ang;
  float dist;
  RayHit hit;
} Ray;


void ray_scan_floor_ceiling();
void ray_scan_walls();

#endif