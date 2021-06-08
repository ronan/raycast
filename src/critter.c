#include "common.h"
#include "critter.h"

Critter g_critters[MAX_CRITTERS];

// Critter critter_init(Point pos, float ang) {
//   Critter out = (Critter) {};
//   body_init(&g_critters[i].body, (Point){4.5, 3.5}, (rand() / (float)RAND_MAX) * M_PI * 2);
//   return out;
// }

void critters_init()
{


  for (int i = 0; i < MAX_CRITTERS; i++) {

    // Lights
    if (i < 16) {
      int x = floor(i / 4) * 2 + 1;
      int y = i % 4 * 2 + 1;
      g_critters[i] = (Critter) {};
      g_critters[i].glow = 0.2;
      body_init(&g_critters[i].body, (Point){x + .5, y + .5}, 0.0);
      g_critters[i].body.radius = 0.3;
      g_critters[i].body.z = 0.75;
      g_critters[i].opacity = 1.0;
    }
    else {
      // float x = (rand() % (MAP_TILES_X / 2)) * 2 + .2;
      // float y = (rand() % (MAP_TILES_Y / 2)) * 2 + .3;
      float x = ((rand() % MAP_CELLS_X) * 2) + 1.5;
      float y = ((rand() % MAP_CELLS_Y) * 2) + 1.5;

      g_critters[i] = (Critter) {};

      body_init(&g_critters[i].body, (Point){4.5, 3.5}, (rand() / (float)RAND_MAX) * M_PI * 2);

      g_critters[i].body.bouncy = 1;
      g_critters[i].body.radius = (rand() / (float)RAND_MAX) * 0.5;
      g_critters[i].body.radius = 0.2;
      g_critters[i].body.height = 2 * g_critters[i].body.radius;
      g_critters[i].body.speed = CRITTER_MAX_SPEED;
      g_critters[i].glow = 0.9;
      g_critters[i].opacity = 1.0;

    }


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
    Critter *b = &g_critters[i];
  }
}

