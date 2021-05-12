#include "common.h"
#include "input.h"
#include "player.h"
#include "map.h"
#include "geometry.h"

Player g_player;

void player_init()
{
  player_set_pos(6.5, 1.5);
  player_set_angle((M_PI_2 + M_PI_4));
  player_set_speed(PLAYER_SPEED);
  g_player.r = .2;
}

void player_tick(float t) {
  float ang = g_input.crawl ? PLAYER_TURN_ANG / 5 : PLAYER_TURN_ANG;
  float speed = g_input.crawl? PLAYER_SPEED / 5 : PLAYER_SPEED;
  if (g_input.rotate_l) {
    player_modify_angle(-ang * t);
  }
  if (g_input.rotate_r) {
    player_modify_angle(ang * t);
  }
  if (g_input.move_f) {
    player_mv(g_player.dir.x * speed * t, g_player.dir.y * speed * t);
  }
  if (g_input.move_b) {
    player_mv(g_player.dir.x * -speed * t, g_player.dir.y * -speed * t);
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
  Point player_pos = point_add(g_player.pos, (Point){delta_x, delta_y});
  Point player_int = point_add(player_pos, point_mult(g_player.dir, g_player.r));

  MapTile tile = map_tile_at_point(player_int);
  if (!map_tile_is_oob(tile) && !map_tile_is_wall(tile))
    {
      player_set_pos(player_pos.x, player_pos.y);
    }
    else {
      // Todo. Resolve colision so that the player slides along the wall
      // float x_diff = player_int.x - player_pos.x;
      // float y_diff = player_int.y - player_pos.y;
      // if (x_diff > 1) {
      //   player_pos.x = player_int.x = player_pos.x + sinf(g_player.ang) * g_player.r;
      // }
      // if (x_diff < -g_player.r) {
      //   player_pos.x = player_int.x = player_pos.x - sinf(g_player.ang) * g_player.r;
      // }

    }
}
