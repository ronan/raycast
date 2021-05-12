#ifndef _PLYR_H_
#define _PLYR_H_

#include "geometry.h"
#include "body.h"

typedef struct Player
{
  Body body;
  angle ang;
  Point pos;
  Point dir;
  Point camera_plane;
  float speed;
  float r;
} Player;

void player_init();
void player_tick(float t);

void player_set_pos(float x, float y);
void player_set_angle(angle a);
void player_set_speed(float speed);
void player_modify_angle(angle a);
void player_mv(float delta_x, float delta_y);

extern Player g_player;

#endif