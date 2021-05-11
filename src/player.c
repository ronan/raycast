#include "common.h"
#include "input.h"
#include "player.h"

Player g_player;

void player_init()
{
  player_set_pos(6.5, 1.5);
  player_set_angle(-M_PI_2);
  player_set_speed(PLAYER_SPEED);
}

void player_tick(float t) {
  if (g_input.rotate_l) {
    player_modify_angle(-PLAYER_TURN_ANG * t);
  }
  if (g_input.rotate_r) {
    player_modify_angle(PLAYER_TURN_ANG * t);
  }
  if (g_input.move_f) {
    player_mv(g_player.dir.x * PLAYER_SPEED * t, g_player.dir.y * PLAYER_SPEED * t);
  }
  if (g_input.move_b) {
    player_mv(g_player.dir.x * -PLAYER_SPEED * t, g_player.dir.y * -PLAYER_SPEED * t);
  }
}

void player_set_pos(float x, float y)
{
  g_player.pos.x = x;
  g_player.pos.y = y;
}

void player_set_angle(angle a)
{
  g_player.ang = a;

  g_player.dir = point_rotate((Point){1, 0}, a);
  g_player.camera_plane = point_rotate((Point){0, FOCAL_LENGTH}, a);
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
