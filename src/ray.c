#include <math.h>

#include "common.h"
#include "gfx.h"
#include "player.h"
#include "scene.h"
#include "map.h"
#include "geometry.h"
#include "viz.h"
#include "render.h"
#include "pixel.h"

#include "ray.h"

float point_dot(Point a, Point b) {
  return a.x * b.x + a.y * b.y;
}

Point ray_circle_intersection(Ray *r, Point c, float radius) {
  Point a = r->start, b = r->end;
  Point d = point_sub(b, a);

  float dl = point_dot(d, d);
  float t = ((c.x - a.x) * d.x + (c.y - a.y) * d.y) / dl;

  Point nearest = (Point){a.x + t * d.x, a.y + t * d.y};
  float dist = point_dist(nearest, c);
  if (dist < radius)
  {
    float dt = sqrt(radius * radius - dist * dist) / sqrt(dl);
    float t1 = t - dt;
    if (t1 > 0 && t1 <= 1)
    {
      return (Point){a.x + t1 * d.x, a.y + t1 * d.y};
    }
  }
  return POINT_OOB; 
}

Point ray_circle_intersection2(Ray r, Point c, float radius) {
  Point d = point_sub(r.end, r.start);
  Point f = point_sub(r.start, c);

  float a = point_dot(d, d);
  float b = 2 * point_dot(f, d);
  float cc = point_dot(f, f) - (radius * radius);

float discriminant = b*b-4*a*cc;
if( discriminant < 0 )
{
  // no intersection
}
else
{
  // ray didn't totally miss sphere,
  // so there is a solution to
  // the equation.
  
  discriminant = sqrt( discriminant );

  // either solution may be on or off the ray so need to test both
  // t1 is always the smaller value, because BOTH discriminant and
  // a are nonnegative.
  float t1 = (-b - discriminant)/(2*a);
  float t2 = (-b + discriminant)/(2*a);

  // 3x HIT cases:
  //          -o->             --|-->  |            |  --|->
  // Impale(t1 hit,t2 hit), Poke(t1 hit,t2>1), ExitWound(t1<0, t2 hit), 

  // 3x MISS cases:
  //       ->  o                     o ->              | -> |
  // FallShort (t1>1,t2>1), Past (t1<0,t2<0), CompletelyInside(t1<0, t2>1)
  
  if( t1 >= 0 && t1 <= 1 )
  {
    // t1 is the intersection, and it's closer than t2
    // (since t1 uses -b - discriminant)
    // Impale, Poke
    return point_mult(r.dir, t1);
  }

  // here t1 didn't intersect so we are either started
  // inside the sphere or completely past it
  if( t2 >= 0 && t2 <= 1 )
  {
    // ExitWound
    return point_mult(r.dir, t2);
  }
  
  // no intn: FallShort, Past, CompletelyInside
  return POINT_OOB ;
}
}
Point ray_circle_intersection1(Ray r, Point c, float radius) {
  // The equation of the line r.start -> r.end is:
  //    x = r.dir.x * t + r.start.x,
  //    y = r.dir.y * t + r.start.y

  // Compute the distance between r.start and e, where
  // e is the point of the ray closest the circle center (c)
  float t = r.dir.x * (c.x-r.start.x) + r.dir.y * (c.y-r.start.y);
  Point e = (Point){t * r.dir.x + r.start.x, t * r.dir.y + r.start.y};

  float d_2 = point_dist_squared(c, e);
  float r_2 = radius * radius;

  // The point is closer to the line than the radius
  if (d_2 <= r_2)
  {
      // compute distance from t to circle intersection point
      float dt = sqrt(r_2 - d_2);

      // compute first intersection point
      return (Point){
        (t-dt) * r.dir.x + r.start.x,
        (t-dt) * r.dir.y + r.start.y,
      };
  }
  return POINT_OOB;
}

Ray ray_invert(Ray r) {
  r.dir = point_invert(r.dir);
  r.end = point_invert(r.end);
  return r;
}

Ray create_ray() {
  return (Ray) {
    .start = g_player.body.pos,
    .end = g_player.body.pos,
    .dir = g_player.body.dir,
    .dist = DAMN_NEAR_INFINITY,
    .hit = RAY_HIT_OOB
  };
}

// Ray casting for horizontal surfaces inspired by lodev
// https://lodev.org/cgtutor/raycasting.html
void ray_floor_ceiling_scan() {
  Ray r = create_ray();

  // Define the start and end ray vectors
  Point player_dir = point_mult(g_player.body.dir, FOCAL_LENGTH);
  Point camera_plane = point_mult(g_player.camera_plane, FOCAL_LENGTH);
  Point ray_dir_from = point_sub(player_dir, camera_plane);
  Point ray_dir_to = point_add(player_dir, camera_plane);

  // Start at 1 below the horizon
  for(int row = SCREEN_HORIZON + 1; row <= SCREEN_H; row++)
  {
    // Distance from the camera to the floor for the current row.
    r.dist = CAMERA_HEIGHT / (row - SCREEN_HORIZON);
 
    // The sample location of the leftmost column.
    r.end = point_add(g_player.body.pos, point_mult(ray_dir_from, r.dist));

    // The step between the sample location for each column of the screen.
    r.dir = point_mult(point_sub(ray_dir_to, ray_dir_from), r.dist/SCREEN_W);

    Pixel px;
    g_gfx.screen_draw.y = row;
    for(int col = 0; col < SCREEN_W; ++col)
    {
        g_gfx.screen_draw.x = col;
        r.hit.local = point_fractional(r.end);
        g_gfx.object_draw = point_mult(r.hit.local, 512);

        px = render_ceiling(r);
        gfx_put_pixel(col, SCREEN_H - row, (SDL_Color)px);
        px = render_floor(r);
        gfx_put_pixel(col, row, (SDL_Color)px);

        // viz_map_dot(r.end, 1, (SDL_Color)px);
        // viz_map_floor_ray(r, px);
        r.end = point_add(r.end, r.dir);
        // viz_map_ray(r);
    }
  }
}

Ray ray_cast_step(Ray r) {
  Ray out = r;
  Point d;

  d.x = r.dir.x > 0 ?
    ceil(r.end.x) + 0.0001 - r.end.x:
    floor(r.end.x) - 0.0001 - r.end.x;

  d.y = d.x * (r.dir.y / r.dir.x);

  out.end = point_add(out.end, d);

  return out;
}

Ray ray_cast_step_v(Ray r) {
  r = ray_cast_step(r);
  viz_map_ray_partial(r);
  return r;
}

Ray ray_wall_cast_step_h(Ray r) {
  // Horizontal steps are the same logic as vertical with the x and y inverted.
  r = ray_invert(r);
  r = ray_cast_step(r);
  r = ray_invert(r);

  viz_map_ray_partial(r);
  return r;
}

Point wall_local_hit_point_from_ray(Ray *r) {
  Point local, out = (Point){0,0};
  local.x = r->end.x - floor(r->end.x);
  local.y = r->end.y - floor(r->end.y);

  if (r->hit.wall == MAP_S || r->hit.wall == MAP_N) {
    out.x = local.x;
  }
  if (r->hit.wall == MAP_E || r->hit.wall == MAP_W) {
    out.x = local.y;
  }

  // if (r->hit.wall == MAP_S || r->hit.wall == MAP_W) {
  //   out.x = 1.0 - out.x;
  // }
  return out;
}

void ray_wall_draw(Ray *r, int col) {
  // Draw a vertical slice of the wall
  int h = (WALL_H * SCREEN_H) / (r->dist);
  int top = SCREEN_Y + (SCREEN_H / 2) - (h / 2);
  r->hit.local = wall_local_hit_point_from_ray(r);
  float uv_delta_y = 1.0 / h;

  g_gfx.screen_draw.x = col;
  for (int row = SCREEN_Y + top; row <= SCREEN_Y + top + h; row++) {
    if (row > 0 && row < SCREEN_H) {
      g_gfx.screen_draw.y = row;
      g_gfx.object_draw = point_mult(r->hit.local, (WALL_H * SCREEN_H) / (r->dist));
      Pixel px = render_wall(*r);
      gfx_put_pixel(col, row, (SDL_Color)px);
    }
    r->hit.local.y += uv_delta_y;
    // Floating point math can put us a weeee bit over 1.0 so we clamp the max to avoid texture wrapping
    r->hit.local.y = r->hit.local.y >= 1.0 ? 0.999: r->hit.local.y;
  }
  return;
}

Ray ray_wall_cast(int col) {
  float h_dist_squared = DAMN_NEAR_INFINITY,
        v_dist_squared = DAMN_NEAR_INFINITY;

  Ray h, v;
  v = h = create_ray();

  Point player_dir = point_mult(g_player.body.dir, g_player.r);
  Point camera_plane = point_mult(g_player.camera_plane, 1  );

  // Set the direction of the ray based on the camera properties and current column
  float camera_x = (2.0 * ((float)col / SCREEN_W)) - 1.0;
  v.dir = h.dir = point_add(v.dir, point_mult(camera_plane, camera_x));

  viz_map_vector(v.start, camera_plane, COLOR_YELLOW);
  viz_map_vector(v.start, v.dir, COLOR_RED);

  for (int i = 0; i < MAP_TILES_X; i++) {
    v = ray_cast_step_v(v);
    v.hit.tile = map_tile_at_point(v.end);
    if (map_tile_is_oob(v.hit.tile)) {
      break;
    }
    if (map_tile_is_wall(v.hit.tile)) {
      v.hit.wall = v.dir.x > 0 ? MAP_W : MAP_E;
      // The perpendicular distance avoids fisheye
      v.dist = (v.end.x - v.start.x) / v.dir.x;
      // The euclidean distance lets us find the closest hit
      v_dist_squared = point_dist_squared(v.start, v.end);
      // Local wall coordinates of hit
      v.hit.local.x = (point_fractional(v.end)).y;

      break;
    }
  }

  for (int i = 0; i < MAP_TILES_Y; i++) {
    h = ray_wall_cast_step_h(h);
    h.hit.tile = map_tile_at_point(h.end);
    if (map_tile_is_oob(h.hit.tile)) {
      break;
    }
    if (map_tile_is_wall(h.hit.tile)) {
      h.hit.wall = h.dir.y > 0 ? MAP_N : MAP_S;
      // The perpendicular distance avoids fisheye
      h.dist = (h.end.y - h.start.y) / h.dir.y;
      // The euclidean distance lets us find the closest hit
      h_dist_squared = point_dist_squared(h.start, h.end);
      // Local wall coordinates of hit
      h.hit.local.x = (point_fractional(h.end)).x;

      break;
    }
  }

  Ray r = h_dist_squared < v_dist_squared ? h : v;
  
  viz_map_ray(r);
  ray_wall_draw(&r, col);

  return r;
}


void ray_scan_walls()
{
  // Draw only 3 rays. For debugging
  // ray_wall_cast(SCREEN_W/2); return;
  // ray_wall_cast(1); ray_wall_cast(SCREEN_W/2); ray_wall_cast(SCREEN_W); return;

  for (int col = 0; col < SCREEN_W; col += 1)
  {
    ray_wall_cast(col);
  }
}