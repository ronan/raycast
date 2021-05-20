#ifndef _CRITTER_H_
#define _CRITTER_H_

#include "common.h"
#include "body.h"

typedef struct Critter {
  Body body;
} Critter;

extern Critter g_critters[MAX_CRITTERS];


void critters_init();
void critters_destroy();
void critters_tick(float t);

#endif