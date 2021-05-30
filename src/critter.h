#ifndef _CRITTER_H_
#define _CRITTER_H_

#include "common.h"
#include "body.h"

typedef struct Critter {
  Body body;

  Point camera_left;
  Point camera_right;
  float camera_dist;
  float camera_ang_cos;

  int screen_top;
  int screen_bot;
  int screen_h;

  float glow;
} Critter;

extern Critter g_critters[MAX_CRITTERS];


void critters_init();
void critters_destroy();
void critters_tick(float t);

#endif