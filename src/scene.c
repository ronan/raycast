#include "common.h"
#include "gfx.h"
#include "player.h"
#include "map.h"
#include "ray.h"
#include "viz.h"

#include "scene.h"

void scn_draw_bg()
{
  gfx_clear(COLOR_CEILING);
}

void scn_draw()
{
  // srand(g_player.body.pos.x*g_player.body.ang*1000 );
  // srand(1);
  scn_draw_bg();

  viz_draw();

  ray_scan();

  gfx_update();
  viz_update();

}