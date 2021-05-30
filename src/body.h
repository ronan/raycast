#ifndef _BODY_H_
#define _BODY_H_

#include "geometry.h"

typedef struct body
{
  Point pos;
  Point dir;
  Point lat;

  angle ang;
  angle ang_velocity;

  int bouncy;

  float speed;
  float lateral_speed;

  float radius;
  float height;
} Body;

extern Point g_camera_plane;
extern Point g_camera_pos;
extern Point g_camera_dir;


void body_init(Body *b, Point pos, float ang);
void body_tick(Body *b, float t);

void body_set_pos(Body *b, Point p);
void body_set_angle(Body *b, angle a);
void body_modify_angle(Body *b, angle a);
void body_set_speed(Body *b, float speed);
void body_mv(Body *b, Point delta);

#endif