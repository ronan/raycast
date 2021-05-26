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

int ray_billboard_intersection(Ray *r, Point c, float radius) {
  Point a = r->start,
        b = r->end;

  Point i = point_sub(b, a);
  Point dir = point_sub(c, a);

  // a->b dot a->c is positive if the player is facing towards the billboard.
  float billboad_dir = point_dot(i, dir);
  float circle_center_dist = point_dist(a, c);
  if (circle_center_dist < r->dist && billboad_dir > 0) {
    // Don't draw outside the bounds of the distance normalized billboard
    float h = (WALL_H * SCREEN_H) / (circle_center_dist);
    float top = SCREEN_HORIZON - (h / 2);
    if (r->pixel.y < top || r->pixel.y > top + h) {
      return 0;
    }

    Point d = point_add(c, point_rotate((Point){0, radius}, g_player.body.ang));
    Point e = point_add(c, point_rotate((Point){0, -radius}, g_player.body.ang));

    Point j = point_sub(a, e);
    Point k = point_sub(a, b);
    Point l = point_sub(e, d);

    // viz_map_line(a, b, COLOR_YELLOW);
    // viz_map_line(e, d, COLOR_MAGENTA);

    // viz_map_vector(a, i, COLOR_RED);
    // viz_map_vector(b, j, COLOR_GREEN);
    // viz_map_vector(b, k, COLOR_BLUE);
    // viz_map_vector(d, l, COLOR_YELLOW);

    float u = ((i.x * j.y) - (i.y * j.x)) /
              ((k.x * l.y) - (k.y * l.x));

    if (u > 0 && u <= 1) {
      r->hit.local.x = u;
      r->hit.local.y = (r->pixel.y - top) / (float)h;
      r->dist = circle_center_dist;
      r->end = point_add(e, point_mult(point_sub(d, e), u));
      // viz_map_dot(r->end, 5, COLOR_RED);
      return 1;
    }
  }
  return 0;
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
    .hit = RAY_HIT_OOB,
    .pixel = POINT_OOB,
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
    r.pixel.y = g_gfx.screen_draw.y = row;

    // Distance from the camera to the floor for the current row.
    r.dist = CAMERA_HEIGHT / (row - SCREEN_HORIZON);
 
    // The sample location of the leftmost column.
    r.end = point_add(g_player.body.pos, point_mult(ray_dir_from, r.dist));

    // The step between the sample location for each column of the screen.
    r.dir = point_mult(point_sub(ray_dir_to, ray_dir_from), r.dist/SCREEN_W);

    Pixel px;
    for(int col = 0; col < SCREEN_W; ++col)
    {
        r.pixel.x = g_gfx.screen_draw.x = col;
        r.hit.local = point_fractional(r.end);

        Ray r_ceiling = r;
        r_ceiling.pixel.y = SCREEN_H - r.pixel.y;
        px = render_ceiling(r_ceiling);
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
  r->pixel.x = g_gfx.screen_draw.x = col;

  // Draw a vertical slice of the wall
  float h = (WALL_H * SCREEN_H) / (r->dist);
  float top = SCREEN_HORIZON - (h / 2.0);
  r->hit.local = wall_local_hit_point_from_ray(r);

  for (int row = top; row <= top + h; row++) {
    if (row > 0 && row < SCREEN_H) {
      r->pixel.y = g_gfx.screen_draw.y = row;
      r->hit.local.y = (float)(row - top) / h;
      r->hit.local.y = r->hit.local.y >= 1.0 ? 0.99 : r->hit.local.y;
      r->hit.local.y = r->hit.local.y <= 0.0 ? 0.0  : r->hit.local.y;
  
      Pixel px = render_wall(*r);
      gfx_put_pixel(col, row, (SDL_Color)px);
    }
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
  // viz_map_vector(v.start, v.dir, COLOR_RED);

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