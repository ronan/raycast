#include "geometry.h"

#include "body.h"
#include "map.h"

Point g_camera_plane;
Point g_camera_pos;
Point g_camera_dir;

Body body_new(Point pos, float ang)
{
  Body b;
  b.bouncy = 0;
  b.speed = 0.0;
  b.ang_velocity = 0.0;
  b.lateral_speed = 0.0;
  b.z = 0;
  b.height = 0;
  body_set_pos(&b, pos);
  body_set_angle(&b, ang);
  return b;
}

void body_tick(Body *b, float t) {
  if (b->speed) {
    body_mv(b, point_mult(b->dir, b->speed * t));
  }
  if (b->lateral_speed) {
    body_mv(b, point_mult(b->lat, b->lateral_speed * t));
  }
  if (b->ang_velocity) {
    body_modify_angle(b, b->ang_velocity * t);
  }
}

void body_set_pos(Body *b, Point p)
{
  b->pos = p;
}

void body_set_angle(Body *b, angle a)
{
  b->ang = a;
  b->dir = point_rotate((Point){1, 0}, a);
  b->lat = point_rotate((Point){0, 1}, a);
}

void body_set_dir(Body *b, Point dir)
{
  b->dir = dir;
  b->ang = atan2(dir.y, dir.x);

}

void body_set_speed(Body *b, float speed)
{
  b->speed = speed;
}

void body_modify_angle(Body *b, angle a)
{
  body_set_angle(b, ang_add(b->ang, a));
}

void body_bounce_x(Body *b, int dir) {
  if (b->bouncy) {
    body_set_dir(b, (Point){-b->dir.x, b->dir.y});
  }
}

void body_bounce_y(Body *b, int dir) {
  if (b->bouncy) {
    body_set_dir(b, (Point){b->dir.x, -b->dir.y});
  }
}

void body_mv(Body *b, Point delta)
{
  MapTile tile;
  Point body_pos, sensor, body_int, local_int, body_tile, diff;

  body_pos = point_add(b->pos, delta);
  if ((int)(body_pos.x + b->radius) > (int)b->pos.x && !map_tile_is_empty(map_tile_at_point((Point){body_pos.x + b->radius, body_pos.y}))) {
    body_pos.x = ceil(b->pos.x) - b->radius;
    body_bounce_x(b, 1);
  }
  if ((int)(body_pos.x - b->radius) < (int)b->pos.x && !map_tile_is_empty(map_tile_at_point((Point){body_pos.x - b->radius, body_pos.y}))) {
    body_pos.x = floor(b->pos.x) + b->radius;
    body_bounce_x(b, -1);
  }
  if ((int)(body_pos.y + b->radius) > (int)(b->pos.y) && !map_tile_is_empty(map_tile_at_point((Point){body_pos.x, body_pos.y + b->radius}))) {
    body_pos.y = ceil(b->pos.y) - b->radius;
    body_bounce_y(b, -1);
  }
  if ((int)(body_pos.y - b->radius) < (int)(b->pos.y) && !map_tile_is_empty(map_tile_at_point((Point){body_pos.x, body_pos.y - b->radius}))) {
    body_pos.y = floor(b->pos.y) + b->radius;
    body_bounce_y(b, 1);
  }

  body_set_pos(b, body_pos);
}

