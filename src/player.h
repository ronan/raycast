#ifndef _PLYR_H_
#define _PLYR_H_

#include "geometry.h"

typedef struct Player
{
  angle ang;
  Point pos;
  Point dir;
  float speed;
} Player;

void player_init();
void player_set_pos(float x, float y);
void player_set_angle(angle a);
void player_set_speed(float speed);
void player_modify_angle(angle a);
void player_mv(float delta_x, float delta_y);
void player_mv_left();
void player_mv_right();
void player_mv_up();
void player_mv_down();

extern Player g_player;

#endif