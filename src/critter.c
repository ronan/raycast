#include "common.h"
#include "particle.h"
#include "critter.h"
#include "utils.h"
#include "map.h"

Critter g_critters[MAX_CRITTERS];
int g_num_critters = 0;

Critter null_critter = (Critter) {
  .body = (Body) {
    .bouncy = 1,
    .radius =  0.0f,
    .height = 0.0f,
    .speed = 0.0f,
  },
  .type = CRITTER_NULL,
  .glow = 0.0f,
  .glow_color = COLOR_WHITE,
};

void critters_init()
{
  g_num_critters = 0;
  g_critters[g_num_critters] = null_critter;

  g_critters[g_num_critters].body = body_new((Point){3.8, 6.4291563}, rand_scaled(M_PI * 2));
  g_critters[g_num_critters].body.radius = 0.25f;
  g_critters[g_num_critters].body.height = 2 * g_critters[g_num_critters].body.radius;
  g_critters[g_num_critters].glow = 0.8;
  g_critters[g_num_critters].type = CRITTER_ORB;
  // return;

  for (; g_num_critters < MAX_CRITTERS; g_num_critters++) {
    // Wall Lights
    if (g_num_critters< MAX_LIGHTS) {
      int x = g_num_critters % 4 * 2 + 1;
      int y = floor(g_num_critters / 4) * 2 + 1;

      Point p = POINT_OOB;
      float radius = 0.04;
      float wall_offset = radius;

      if (((x + y)/2) % 2 == 0) {
        if (map_tile_is_wall(map_tile_at_point((Point){x, y - 1}))) {
          p = (Point){x + 0.5, y + wall_offset};
        }
        else if (map_tile_is_wall(map_tile_at_point((Point){x - 1, y}))) {
          p = (Point){x + wall_offset, y + 0.5};
        }
      }
      else {
        if (map_tile_is_wall(map_tile_at_point((Point){x, y + 1}))) {
          p = (Point){x + 0.5, y + 1 - wall_offset};
        }
        else if (map_tile_is_wall(map_tile_at_point((Point){x + 1, y}))) {
          p = (Point){x + 1 - wall_offset, y + 0.5};
        }
      }

      g_critters[g_num_critters] = (Critter) {};
      g_critters[g_num_critters].body = body_new(p, 0.0);
      g_critters[g_num_critters].glow = 0.1;
      g_critters[g_num_critters].glow_color = (SDL_Color) {255, 183, 80, 255};
      g_critters[g_num_critters].body.height = radius * 2;
      g_critters[g_num_critters].body.radius = radius;
      g_critters[g_num_critters].body.z = 0.5;
      g_critters[g_num_critters].type = CRITTER_LIGHT;
    }
    // Ceiling Lights
    // else if (i < 16) {
    //   int x = floor(i / 4) * 2 + 1;
    //   int y = i % 4 * 2 + 1;
    //   g_critters[i] = (Critter) {};
    //   g_critters[i].glow = 0.2;
    //   g_critters[i].body = body_new((Point){x + .5, y + .5}, 0.0);
    //   g_critters[i].body.height = g_critters[i].body.radius = 0.2;
    //   g_critters[i].body.z = 0.8;
    //   g_critters[i].type = CRITTER_LIGHT;
    // }
    else {
      float x = (rand_int(MAP_CELLS_X) * 2) + 1.5;
      float y = (rand_int(MAP_CELLS_Y) * 2) + 1.5;

      g_critters[g_num_critters] = (Critter) {};
      g_critters[g_num_critters].body = body_new((Point){x, y}, rand_scaled(M_PI * 2));
      g_critters[g_num_critters].body.bouncy = 1;
      g_critters[g_num_critters].body.radius = 0.2;
      g_critters[g_num_critters].body.height = 2 * g_critters[g_num_critters].body.radius;
      g_critters[g_num_critters].body.speed = CRITTER_MAX_SPEED;
      g_critters[g_num_critters].glow = 0.2;
      g_critters[g_num_critters].glow_color = (SDL_Color) {255, 255, 255, 255};
      g_critters[g_num_critters].type = CRITTER_ORB;
    }
  }
}

void critters_destroy()
{
}

void critters_tick(float t)
{
  for (int i = 0; i < g_num_critters; i++) {
    if (g_critters[i].type == CRITTER_ORB) {
      g_critters[i].body.ang_velocity = rand_perturb(0, CRITTER_MAX_TURN);
    }
    if (g_critters[i].type == CRITTER_LIGHT) {
      g_critters[i].glow = rand_perturb(0.5, 0.05);

      if (g_tick % 500) {
        particles_emit_smoke((Point3){.x = g_critters[i].body.pos.x, .y = g_critters[i].body.pos.y, .z = g_critters[i].body.z + 0.10});
      }
      if (g_tick % 200) {
        particles_emit_fire((Point3){.x = g_critters[i].body.pos.x, .y = g_critters[i].body.pos.y, .z = g_critters[i].body.z + 0.10});
      }
    }

    body_tick(&g_critters[i].body, t);
  }
}

