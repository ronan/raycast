#ifndef _RAY_H_
#define _RAY_H_

#include "map.h"


typedef enum RayHitType {
  HIT_NONE,
  HIT_WALL,
  HIT_CEIL,
  HIT_FLOOR,
  HIT_CRITTER
} RayHitType;

typedef struct RayHit {
  MapTile tile;
  MapDir wall;
  Point local;
  RayHitType type;
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

#define DAMN_NEAR_INFINITY 10000
#define RAY_HIT_OOB (RayHit) {.tile = MAP_TILE_OOB, .local = POINT_ORIGIN, .type = HIT_NONE}
#define RAY_OOB (Ray) {.pos = POINT_OOB, .len = DAMN_NEAR_INFINITY, .hit = RAY_HIT_OOB};

void ray_scan();

#endif