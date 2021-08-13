#ifndef _RAY_H_
#define _RAY_H_

#include "common.h"
#include "map.h"
#include "critter.h"


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
  Critter *critter;
} RayHit;

typedef struct Ray {
  Point start;
  Point end;
  Point dir;
  Point pixel;
  angle ang;
  float dist;
  RayHit hit;
  // 0 (floor) to 1 (ceiling)
  float z;
} Ray;

typedef struct CritterHit {
  Critter c;
  Ray r;
} CritterHit;


#define RAY_HIT_OOB (RayHit) {.tile = MAP_TILE_OOB, .local = POINT_ORIGIN, .type = HIT_NONE}
#define RAY_OOB (Ray) {.pos = POINT_OOB, .len = DAMN_NEAR_INFINITY, .hit = RAY_HIT_OOB};

void ray_scan();

#define NOVIZ_RAY_CRITTER
#define NOVIZ_RAY_WALL
#define NOVIZ_RAY_FLOOR

#endif