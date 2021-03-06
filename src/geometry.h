#ifndef _GEOM_H_
#define _GEOM_H_

#include <math.h>

#define ANG_E 0
#define ANG_N 1.57079632679 
#define ANG_W 3.14159265359
#define ANG_S 4.71238898038

#define POINT_ORIGIN (Point) {.x = 0, .y = 0}
#define POINT_OOB (Point) {.x = -1, .y = -1}
#define POINT3_OOB (Point3) {.x = -1, .y = -1, .z = -1}

// An angle in radians
typedef float angle;

// A 2d vector or point
typedef struct Point
{
  float x;
  float y;
} Point;

typedef struct Point3
{
  float x;
  float y;
  float z;
} Point3;

angle ang_add(angle a, angle b);
Point ang_vector(angle a, float scale);
int ang_is_westward(angle a);
int ang_is_northward(angle a);

Point point_add(Point a, Point b);
Point point_sub(Point a, Point b);
Point point_mult(Point a, float scale);
Point point_invert(Point a);
Point point_rotate(Point a, angle b);
Point point_fractional(Point a);
Point point_integral(Point a);
float point_dot(Point a, Point b);
float point_cross(Point a, Point b);
float point_dist_squared(Point from, Point to);
float point_dist(Point from, Point to);
Point point_dir(Point a, Point b);
float point_vector_length(Point to);
Point point_vector_normalize(Point a);

int point_is_oob(Point p);


Point3 point3_add(Point3 a, Point3 b);
Point point3_to_point(Point3 a);
Point3 point3_mult(Point3 a, float scale);

#endif