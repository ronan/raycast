#include "geometry.h"

#include "body.h"
#include "map.h"

void body_init(Body *b, Point pos, float ang)
{
  body_set_pos(b, pos);
  body_set_angle(b, ang);
}


void body_tick(Body *b, float t) {
  if (b->speed) {
    body_mv(b, point_mult(b->dir, b->speed * t));
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
}

void body_set_speed(Body *b, float speed)
{
  b->speed = speed;
}

void body_modify_angle(Body *b, angle a)
{
  body_set_angle(b, ang_add(b->ang, a));
}

void body_mv(Body *b, Point delta)
{
  Point body_pos = point_add(b->pos, delta);
  Point body_int = point_add(body_pos, point_mult(b->dir, b->r));

  MapTile tile = map_tile_at_point(body_int);
  if (!map_tile_is_oob(tile) && !map_tile_is_wall(tile))
    {
      body_set_pos(b, body_pos);
    }
    else {
      // Todo. Resolve colision so that the player slides along the wall
    }
}