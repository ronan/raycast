#include "common.h"
#include "gfx.h"
#include "player.h"
#include "map.h"
#include "ray.h"
#include "floor.h"

#include "scene.h"

gfx_err scn_minimap_draw_player()
{
  return GFX_ERR_NONE;
}

gfx_err scn_draw_bg()
{
  gfx_clear(COLOR_CEILING);
  return GFX_ERR_NONE;
}

gfx_err scn_draw_minimap()
{
  int x, y, xo, yo;
  SDL_Color *color;
  for (y = 0; y < MAP_TILES_Y; y++)
  {
    for (x = 0; x < MAP_TILES_X; x++)
    {
      color = (g_map[y][x] ? &COLOR_WALL : &COLOR_FLOOR);
      gfx_put_rect(x * MINIMAP_SCALE + 1, y * MINIMAP_SCALE + 1, MINIMAP_SCALE - 1, MINIMAP_SCALE - 1, *color);
    }
  }

  // Draw player
  float line_l = MINIMAP_SCALE / 3;
  Point from = point_mult(g_player.pos, MINIMAP_SCALE);
  Point to = point_add(from, point_mult(g_player.dir, line_l));

  gfx_put_line(from.x, from.y, to.x, to.y, COLOR_GREEN);
  gfx_put_pixel(from.x, from.y, COLOR_RED);

  return GFX_ERR_NONE;
}

gfx_err scn_draw_ceiling_and_floor()
{
  float half_h = (SCREEN_H / 2);
  float middle_y = SCREEN_Y + half_h;
  gfx_put_rect(SCREEN_X, SCREEN_Y, SCREEN_W, middle_y, COLOR_CEILING);
  gfx_put_rect(SCREEN_X, middle_y, SCREEN_W, half_h, COLOR_FLOOR);

  ray_scan_floor_ceiling();

  return GFX_ERR_NONE;
}

gfx_err scn_draw()
{
  scn_draw_bg();
  scn_draw_minimap();
  scn_draw_ceiling_and_floor();
  // ray_scan_floor_ceiling();
  ray_scan_walls();
  gfx_update();
  return GFX_ERR_NONE;
}