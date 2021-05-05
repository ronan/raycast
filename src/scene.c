#include "common.h"
#include "gfx.h"
#include "player.h"
#include "map.h"
#include "ray.h"

#include "scene.h"

gfx_err scn_draw_player()
{
  float to_x, to_y, line_l = 20.0;
  to_x = g_player.pos.x + (g_player.dir.x * line_l);
  to_y = g_player.pos.y + (g_player.dir.y * line_l);
  gfx_putline(g_player.pos.x, g_player.pos.y, to_x, to_y, COLOR_GREEN);
  gfx_putpixel(g_player.pos.x, g_player.pos.y, COLOR_RED);
  return GFX_ERR_NONE;
}

gfx_err scn_draw_bg()
{
  gfx_clear(COLOR_BG);
  return GFX_ERR_NONE;
}

gfx_err scn_draw_map()
{
  int x, y, xo, yo;
  SDL_Color *color;
  for (y = 0; y < MAP_TILES_Y; y++)
  {
    for (x = 0; x < MAP_TILES_X; x++)
    {
      color = (g_map[y][x] ? &COLOR_WALL : &COLOR_FLOOR);
      gfx_putrect(x * MAP_TILES_S + 1, y * MAP_TILES_S + 1, MAP_TILES_S - 1, MAP_TILES_S - 1, *color);
    }
  }

  return GFX_ERR_NONE;
}

gfx_err scn_draw_ceiling_and_floor()
{
  float half_h = (SCREEN_H / 2);
  float middle_y = SCREEN_Y + half_h;
  gfx_putrect(SCREEN_X, SCREEN_Y, SCREEN_W, middle_y, COLOR_CEILING);
  gfx_putrect(SCREEN_X, middle_y, SCREEN_W, half_h, COLOR_FLOOR);
  return GFX_ERR_NONE;
}

gfx_err scn_draw()
{
  scn_draw_bg();
  scn_draw_map();
  scn_draw_ceiling_and_floor();
  ray_draw_all();
  scn_draw_player();
  gfx_update();
  return GFX_ERR_NONE;
}