#include <math.h>

#include "geometry.h"

angle ang_wrap(angle a)
{
  if (a >= 2 * M_PI)
  {
    a -= 2 * M_PI;
  }
  if (a < 0)
  {
    a += 2 * M_PI;
  }
  return a;
}

angle ang_add(angle a, angle b)
{
  return ang_wrap(a + b);
}

int ang_is_westward(angle a) {
  return ANG_N < a && a < ANG_S;
}

int ang_is_northward(angle a) {
  return ANG_E < a && a < ANG_W;
}


Point ang_vector(angle a, float scale)
{
  return (Point) {.x = cos(a) * scale, .y = sin(a) * -scale};
}

Point point_add(Point a, Point b) {
  return (Point) {.x = a.x + b.x, .y = a.y + b.y};
}

Point point_mult(Point a, float scale)
{
  return (Point) {.x = a.x * scale, .y = a.y * scale};
}
