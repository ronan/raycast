#include "common.h"
#include "critter.h"

Critter g_critters[MAX_CRITTERS];

void critters_init()
{
  for (int i = 0; i < MAX_CRITTERS; i++) {
    // float x = (rand() % (MAP_TILES_X / 2)) * 2 + .2;
    // float y = (rand() % (MAP_TILES_Y / 2)) * 2 + .3;
    float x = ((rand() % MAP_CELLS_X) * 2) + 1.5;
    float y = ((rand() % MAP_CELLS_Y) * 2) + 1.5;

    g_critters[i] = (Critter) {};

    x = 4;
    y = 4;
    body_init(&g_critters[i].body, (Point){x, y}, (rand() % 64) * (M_2_PI / 64));

    g_critters[i].body.bouncy = 1;
    g_critters[i].body.radius = CRITTER_RADIUS;
    g_critters[i].body.speed = CRITTER_MAX_SPEED;
  }
}

void critters_destroy()
{
}

void critters_tick(float t)
{
  for (int i = 0; i < MAX_CRITTERS; i++) {
    g_critters[i].body.ang_velocity = ((float)rand()/RAND_MAX * CRITTER_MAX_TURN - (CRITTER_MAX_TURN/2));
    body_tick(&g_critters[i].body, t);
  }
}

