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

float point_dist_squared(Point from, Point to)
{
  float x = to.x - from.x;
  float y = to.y - from.y;
  return (x * x) + (y * y);
}

float point_dist(Point from, Point to)
{
  return sqrt(point_dist_squared(from, to));
}

float point_vector_length(Point to)
{
  return point_dist((Point){0,0}, to);
}

Point point_vector_normalize(Point a) {
  return point_mult(a, 1/point_vector_length(a));
}

float point_dot(Point a, Point b) {
  return a.x * b.x + a.y * b.y;
}

float point_cross(Point a, Point b) {
  return a.x * b.y - a.y * b.x;
}

Point point_dir(Point a, Point b) {
  return point_mult(point_sub(b, a), 1/point_dist(a, b));
}


Point point_rotate(Point a, angle b) {
  // 𝑥2=cos𝛽𝑥1−sin𝛽𝑦1
  // 𝑦2=sin𝛽𝑥1+cos𝛽𝑦1
  float c = cos(b), s = sin(b);
  return (Point) {.x = c * a.x - s * a.y, s * a.x + c * a.y};
}

Point point_add(Point a, Point b) {
  return (Point) {.x = a.x + b.x, .y = a.y + b.y};
}

Point point_sub(Point a, Point b) {
  return (Point) {.x = a.x - b.x, .y = a.y - b.y};
}

Point point_mult(Point a, float scale)
{
  return (Point) {.x = a.x * scale, .y = a.y * scale};
}

Point point_invert(Point a)
{
  return (Point) {.x = a.y, .y = a.x};
}

Point point_fractional(Point a)
{
  return point_sub(a, point_integral(a));
}

Point point_integral(Point a)
{
  return (Point) {.x = floor(a.x), .y = floor(a.y)};
}

int point_is_oob(Point p) {
  return p.x == POINT_OOB.x;
}


Point3 point3_add(Point3 a, Point3 b) {
  return (Point3) {.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z};
}

Point3 point3_mult(Point3 a, float scale) {
  return (Point3) {.x = a.x * scale, .y = a.y * scale, .z = a.z * scale};
}

Point point3_to_point(Point3 a) {
  return (Point) {.x = a.x, .y = a.y};
}