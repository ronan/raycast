#include "geometry.h"

#include <math.h>

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