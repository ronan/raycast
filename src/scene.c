#include "common.h"
#include "gfx.h"
#include "player.h"
#include "map.h"
#include "ray.h"
#include "viz.h"

#include "scene.h"

void scn_draw_bg()
{
  gfx_clear(COLOR_BLACK);
}

void scn_draw()
{
  // srand(g_player.body.pos.x*g_player.body.ang*1000 );
  scn_draw_bg();
  ray_scan();

  viz_draw();

  viz_update();
  gfx_update();
}