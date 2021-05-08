// Top down map visualization for debugging
#include "common.h"
#include "map.h"
#include "geometry.h"
#include "gfx.h"
#include "ray.h"
#include "player.h"
#include "scene.h"

#include "viz.h"

gfx_ctx g_viz;

gfx_err viz_init(unsigned int width, unsigned int height)
{
  return gfx_init_window(&g_viz, width, height, WINDOW_W + 150, 100, "Viz");
}

void viz_destroy()
{
  SDL_DestroyRenderer(g_viz.renderer);
  SDL_DestroyWindow(g_viz.window);
}

gfx_err viz_update()
{
  SDL_RenderPresent(g_viz.renderer);
  return GFX_ERR_NONE;
}


gfx_err viz_put_line(int x1, int y1, int x2, int y2, SDL_Color color)
{
  SDL_SetRenderDrawColor(g_viz.renderer, color.r, color.g, color.b, color.a);
  SDL_RenderDrawLine(g_viz.renderer, x1, y1, x2, y2);
  return GFX_ERR_NONE;
}

gfx_err viz_put_rect(int x1, int y1, int w, int h, SDL_Color color)
{
  SDL_Rect rect;
  rect.x = x1;
  rect.y = y1;
  rect.w = w;
  rect.h = h;

  SDL_SetRenderDrawColor(g_viz.renderer, color.r, color.g, color.b, color.a);
  SDL_RenderFillRect(g_viz.renderer, &rect);
  return GFX_ERR_NONE;
}

gfx_err viz_put_dot(Point center, float size, SDL_Color color)
{
  SDL_Rect rect;
  float half_size = size/2;
  rect.x = center.x - half_size;
  rect.y = center.y - half_size;
  rect.w = size;
  rect.h = size;

  SDL_SetRenderDrawColor(g_viz.renderer, color.r, color.g, color.b, color.a);
  SDL_RenderFillRect(g_viz.renderer, &rect);
  return GFX_ERR_NONE;
}

void viz_map() {
  int x, y, xo, yo;
  SDL_Color *color;

  SDL_Color bg = COLOR_BLACK;
  SDL_SetRenderDrawColor(g_viz.renderer, bg.r, bg.g, bg.b, bg.a);
  SDL_RenderClear(g_viz.renderer);

  for (y = 0; y < MAP_TILES_Y; y++)
  {
    for (x = 0; x < MAP_TILES_X; x++)
    {
      color = (g_map[y][x] ? &COLOR_WALL : &COLOR_FLOOR);
      viz_put_rect(x * VIZ_MAP_SCALE + 1, y * VIZ_MAP_SCALE + 1, VIZ_MAP_SCALE - 1, VIZ_MAP_SCALE - 1, *color);
    }
  }

  // Draw player
  float line_l = VIZ_MAP_SCALE / 3;
  Point from = point_mult(g_player.pos, VIZ_MAP_SCALE);
  Point to = point_add(from, point_mult(g_player.dir, line_l));

  viz_put_line(from.x, from.y, to.x, to.y, COLOR_GREEN);
  viz_put_dot(from, 1, COLOR_RED);
}

void viz_map_line(Point a, Point b, SDL_Color c) {
  a = point_mult(a, VIZ_MAP_SCALE);
  b = point_mult(b, VIZ_MAP_SCALE);

  // Don't draw off the map
  if (b.x < VIZ_MAP_W) {
    viz_put_line(a.x, a.y, b.x, b.y, c);
  }
}

void viz_map_vector(Point a, Point b, SDL_Color c) {
  a = point_mult(a, VIZ_MAP_SCALE);
  b = point_mult(b, VIZ_MAP_SCALE);
  b = point_add(a, b);
  viz_map_line(a, b, c);
}

void viz_map_dot(Point a, float size, SDL_Color c) {
  a = point_mult(a, VIZ_MAP_SCALE);
  viz_put_dot(a, size, c);
}

void viz_map_ray(Ray r) {
  return;
  viz_map_line(r.start, r.end, COLOR_YELLOW);
  viz_map_dot(r.end, 5, COLOR_GREEN);
}

void viz_map_ray_partial(Ray r) {
  return;
  viz_map_line(r.start, r.end, COLOR_CYAN);
  viz_map_dot(r.end, 5, COLOR_RED);
}

