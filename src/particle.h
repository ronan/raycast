#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "geometry.h"
#include "pixel.h"
#include "common.h"


typedef struct Particle
{
  int alive;

  Point3 pos;
  Point3 vel;
  Point3 acc;

  // The tick that the particle was emitted
  int t_start;
  // The tick that the particle finally dies
  int t_end;

  Pixel color_start;
  Pixel color_end;
  Pixel color;

  float radius_start;
  float radius_end;
  float radius;

  float camera_space_radius;
  float camera_space_dist;
  Point camera_space_pos;
} Particle;

extern Particle g_particles[MAX_PARTICLES];

Particle partices_new();
void partices_emit(Particle p);
void particles_init();
void particles_destroy();
void particles_tick(float t);
int particle_is_alive(Particle *p);

void particles_emit_fire(Point3 emitter);
void particles_emit_smoke(Point3 emitter);

#endif