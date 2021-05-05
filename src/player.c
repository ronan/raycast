#include "common.h"
#include "player.h"

Player g_player;

void player_init()
{
  player_set_pos(150, 360);
  player_set_angle((7 * M_PI) / 4);
  player_set_speed(PLAYER_SPEED);
}

void player_set_pos(float x, float y)
{
  g_player.pos.x = x;
  g_player.pos.y = y;
}

void player_set_angle(angle a)
{
  g_player.ang = a;
  g_player.dir.x = cos(g_player.ang);
  g_player.dir.y = -sin(g_player.ang);
}

void player_set_speed(float speed)
{
  g_player.speed = speed;
}

void player_modify_angle(angle a)
{
  player_set_angle(ang_add(g_player.ang, a));
}

void player_mv(float delta_x, float delta_y)
{
  player_set_pos(g_player.pos.x + delta_x, g_player.pos.y + delta_y);
}

void player_mv_left()
{
  player_modify_angle(PLAYER_TURN_ANG);
}

void player_mv_right()
{
  player_modify_angle(-PLAYER_TURN_ANG);
}

void player_mv_up()
{
  player_mv(g_player.dir.x * PLAYER_SPEED, g_player.dir.y * PLAYER_SPEED);
}

void player_mv_down()
{
  player_mv(g_player.dir.x * -PLAYER_SPEED, g_player.dir.y * -PLAYER_SPEED);
}
