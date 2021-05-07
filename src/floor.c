#include "common.h"
#include "geometry.h"
#include "player.h"
#include "floor.h"
#include "bitmap.h"
#include "ray.h"

void map_draw_point(Point a) {
  return;
  gfx_put_square_centered(a, 5, COLOR_GREEN);
}

void floor_map_draw_line(Point a, Point b, SDL_Color c) {
  return;
  // Don't draw off the map
  if (b.x < MAP_WIDTH) {
    gfx_put_line(a.x, a.y, b.x, b.y, c);
    gfx_put_square_centered(b, 2, c);
  }
}

void floor_map_draw_vector(Point a, Point b, SDL_Color c) {
  return;
  b = point_mult(b, 32);
  b = point_add(a, b);
  floor_map_draw_line(a, b, c);
}

void floor_draw() {
  Point ray_dir_0, ray_dir_1, floor_pos, floor_step, uv;
  Point plane = (Point){0, 0.6};
  Point pos = point_mult(g_player.pos, 1.0/64);
  Point dir = g_player.dir;

  plane = point_rotate(plane, -g_player.ang);


  //FLOOR CASTING
  for(int y = (SCREEN_H / 2) + 1; y < SCREEN_H; y++)
  {
    // rayDir for leftmost ray (x = 0) and rightmost ray (x = w)
    ray_dir_0 = point_sub(dir, plane);
    ray_dir_1 = point_add(dir, plane);

    // Vertical position of the camera.
    float posZ = 0.5 * SCREEN_H;

    // Horizontal distance from the camera to the floor for the current row.
    float rowDistance = posZ / (y - posZ);

    // calculate the real world step vector we have to add for each x (parallel to camera plane)
    // adding step by step avoids multiplications with a weight in the inner loop
    floor_step = point_mult(point_sub(ray_dir_1, ray_dir_0), rowDistance);
    floor_step.x /= SCREEN_W;
    floor_step.y /= SCREEN_W;

    // real world coordinates of the leftmost column. This will be updated as we step to the right.
    Point ray = point_mult(ray_dir_0, rowDistance);
    floor_pos = point_add(pos, ray);

    floor_map_draw_vector(g_player.pos, ray_dir_0, COLOR_RED);
    floor_map_draw_vector(g_player.pos, ray_dir_1, COLOR_BLUE);
    floor_map_draw_vector(g_player.pos, g_player.dir, COLOR_BLACK);
    floor_map_draw_vector(g_player.pos, plane, COLOR_YELLOW);

    for(int x = 0; x < SCREEN_W; ++x)
    {
      // gfx_put_square_centered(point_mult(floor_pos, 64), 1, COLOR_GREEN);
      //gfx_put_square_centered(floor_pos, 1, COLOR_GREEN);

      // Get the fractional part of the position to determine the texture coordinates
      uv = point_fractional(floor_pos);

      Pixel p = bitmap_sample(BITMAP_FLOOR, uv);
      gfx_put_pixel(SCREEN_X + x, y, (SDL_Color) *p);

      floor_pos = point_add(floor_pos, floor_step);
    }
  }
}