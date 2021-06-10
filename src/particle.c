#include "common.h"
#include "pixel.h"
#include "particle.h"
#include "utils.h"

Particle g_particles[MAX_PARTICLES];
int g_particle_write_idx = 0;
int g_particle_read_idx = 0;

float g_particle_tick = 0.0;

Particle partices_new() {
  return (Particle) {
    .pos = POINT3_OOB,
    .vel = (Point3){0,0,0},
    .acc = (Point3){0,0,0},

    .t_start = 0,
    .t_end = 0,

    .color_start = COLOR_CLEAR,
    .color_end = COLOR_CLEAR,
    .color = COLOR_CLEAR,

    .radius_start = 0.0,
    .radius_end = 0.0,
    .radius = 0.0,

    .alive = 0,

    .camera_space_radius = 0,
    .camera_space_dist = DAMN_NEAR_INFINITY,
    .camera_space_pos = (Point){0,0}
  };
}


void particles_emit(Particle p) {
  p.alive = 1;
  g_particles[g_particle_write_idx] = p;
  g_particle_write_idx = (g_particle_write_idx + 1) % MAX_PARTICLES;
}

void particles_init()
{
  for (int i = 0; i < MAX_PARTICLES; i++) {
    g_particles[i] = partices_new();
  }
}

void particles_destroy()
{
}

int particle_is_alive(Particle *p) {
  return p->t_start < g_particle_tick && p->t_end > g_particle_tick;
}

void particles_emit_smoke(Point3 emitter) {
    Particle p = partices_new();
    p.pos = emitter;
    p.vel = (Point3){.x = rand_perturb(0, 0.01), .y = rand_perturb(0, 0.01), .z = rand_perturb(0.02, 0.01)};
    p.acc = (Point3){.x = rand_perturb(0, 0.001), .y = rand_perturb(0, 0.001), .z = rand_perturb(0.002, 0.001)};
    p.radius_end = p.radius_start = p.radius = rand_range(0.01, 0.1);
    p.radius_end = p.radius * 10;
    p.color_end = p.color = p.color_start = COLOR_BLACK;
    p.color_end = (SDL_Color){ 255, 255, 255, 0 };
    p.t_start = g_particle_tick;
    p.t_end = p.t_start + 30;
    particles_emit(p);
}

void particles_emit_fire(Point3 emitter) {
    Particle p = partices_new();
    p.pos = emitter;
    p.vel = (Point3){.x = rand_perturb(0, 0.01), .y = rand_perturb(0, 0.01), .z = rand_perturb(0.02, 0.01)};
    p.acc = (Point3){.x = rand_perturb(0, 0.001), .y = rand_perturb(0, 0.001), .z = rand_perturb(0.002, 0.001)};
    p.radius_end = p.radius_start = p.radius = rand_range(0.01, 0.1);
    p.color_end = p.color = p.color_start = COLOR_YELLOW;
    p.color_end = COLOR_RED;
    p.t_start = g_particle_tick;
    p.t_end = p.t_start + 15;
    particles_emit(p);
}


void particles_tick(float t)
{
  g_particle_tick += t;

  // if (!g_particles[0].alive) {
  //   Particle p = partices_new();
  //   // p.pos = (Point3){.x = rand_scaled(15), .y = rand_scaled(15), .z = 0};
  //   p.pos = (Point3){.x = 4.5, .y = 3.5, .z = 0.5};
  //   // p.vel = (Point3){.x = rand_range(-0.3, 0.3), .y = rand_range(-0.3, 0.3), .z = 0};
  //   // p.vel = (Point3){.x = rand_range(-0.1, 0.1), .y = rand_range(-0.1, 0.1), .z = 0};
  //   p.radius_end = p.radius_start = p.radius = 0.25;//rand_range(0.01, 0.02);
  //   // p.size_end = p.size * 10;
  //   p.color_end = p.color = p.color_start = (SDL_Color){ 255, 255, 255, 128 };
  //   // p.color_end = (SDL_Color){ 255, 255, 255, 0 };
  //   p.t_start = g_particle_tick;
  //   p.t_end = p.t_start + 600000;
  //   particles_emit(p);
  // }

  if ((int)g_particle_tick % 1 == 0) {
    // particles_emit_smoke((Point3){.x = 4.5, .y = 3.5, .z = 0});
    // particles_emit_fire((Point3){.x = 4.5, .y = 3.5, .z = 0});

    // for (int i = 0; i < MAX_LIGHTS; i++) {
    //       particles_emit_smoke((Point3){.x = 4.5, .y = 3.5, .z = 0});
    //   particles_emit_fire((Point3){.x = g_cr, .y = 3.5, .z = 0});

    // }

  }

  for (int i = 0; i < MAX_PARTICLES; i++) {
    if (particle_is_alive(&g_particles[i])) {
      // The particle age / lifetime range: 0 (emit) -> 1 (die)
      float age_normal = ((float)g_particle_tick - g_particles[i].t_start) / ((float)g_particles[i].t_end - g_particles[i].t_start);

      g_particles[i].vel = point3_add(g_particles[i].vel, g_particles[i].acc);
      g_particles[i].pos = point3_add(g_particles[i].pos, g_particles[i].vel);

      float radius_diff = g_particles[i].radius_end - g_particles[i].radius_start;
      g_particles[i].radius = g_particles[i].radius_start + (radius_diff * age_normal);

      g_particles[i].color = pixel_lerp_linear(g_particles[i].color_start, g_particles[i].color_end, age_normal);
    }
  }
}
