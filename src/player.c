#include "common.h"
#include "input.h"
#include "player.h"
#include "map.h"
#include "geometry.h"
#include "critter.h"

Player g_player;

void player_init()
{
  g_player.body = body_new((Point){7.5, 5.5}, 0);
  player_set_angle(-M_PI_2);
  g_player.body.radius = .2;
  g_player.body.bouncy = 0;
}

void player_tick(float t) {
  float ang = g_input.crawl ? PLAYER_TURN_ANG / 5 : PLAYER_TURN_ANG;
  g_player.body.ang_velocity = 0;
  if (g_input.rotate_l) {
    g_player.body.ang_velocity = -ang;
  }
  if (g_input.rotate_r) {
    g_player.body.ang_velocity = ang;
  }
  
  g_player.body.speed = g_player.body.lateral_speed = 0;
  float speed = g_input.crawl ? PLAYER_SPEED / 5 : PLAYER_SPEED;
  if (g_input.move_f) {
    g_player.body.speed = speed;
  }
  if (g_input.move_b) {
    g_player.body.speed = -speed;
  }

  if (g_input.strafe_l) {
    g_player.body.lateral_speed = -speed;
  }
  if (g_input.strafe_r) {
    g_player.body.lateral_speed = speed;
  }

  body_tick(&g_player.body, t);
  g_critters[0].body.pos = g_player.body.pos;

  g_camera_plane = g_player.camera_plane = point_rotate((Point){0, 1}, g_player.body.ang);
  g_camera_pos = g_player.body.pos;
  g_camera_dir = g_player.body.dir;
}

void player_set_pos(float x, float y)
{
  g_player.body.pos.x = x;
  g_player.body.pos.y = y;
}

void player_set_angle(angle a)
{
  g_player.body.ang = a;
  g_player.body.dir = point_rotate((Point){1, 0}, a);
}

void player_set_speed(float speed)
{
  g_player.body.speed = speed;
}

void player_modify_angle(angle a)
{
  player_set_angle(ang_add(g_player.body.ang, a));
}

void player_mv(float delta_x, float delta_y)
{
  Point player_pos = point_add(g_player.body.pos, (Point){delta_x, delta_y});
  Point player_int = point_add(player_pos, point_mult(g_player.body.dir, g_player.r));

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
      //   player_pos.x = player_int.x = player_pos.x + sinf(g_player.body.ang) * g_player.r;
      // }
      // if (x_diff < -g_player.r) {
      //   player_pos.x = player_int.x = player_pos.x - sinf(g_player.body.ang) * g_player.r;
      // }

    }
}
