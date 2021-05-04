#include "common.h"
#include "gfx.h"
#include "player.h"
#include "map.h"
#include "scene.h"

#include "ray.h"

void ray_draw(r) {

}

void ray_draw_all()
{
  int r, r_min, r_max, mx, my, mp, dof, side;
  float vx, vy, rx, ry, xo, yo, disV, disH, Tan;
  angle ra, raStart, rDelta;
  SDL_Color *color;

  raStart = ang_add(g_player.ang, FOV / 2);
  rDelta = FOV / SCREEN_W;
  int r_x_min = 0;
  int r_x_max = SCREEN_W;

  for (r = r_x_min; r < r_x_max; r++)
  {
    ra = ang_add(raStart, (r * -rDelta));

    // // VERTICAL
    // // Check ray intersection with vertical (north-south) wall segments
    dof = 0;
    disV = 100000;
    float Tan = tan(ra);
    
    // Looking west(ish)
    if (cos(ra) > 0.001)
    {
      rx = (((int)g_player.pos.x >> 6) << 6) + 64;
      ry = (g_player.pos.x - rx) * Tan + g_player.pos.y;
      xo = 64;
      yo = -xo * Tan;
    } 
    // Looking east(ish)
    else if (cos(ra) < -0.001)
    {
      rx = (((int)g_player.pos.x >> 6) << 6) - 0.0001;
      ry = (g_player.pos.x - rx) * Tan + g_player.pos.y;
      xo = -64;
      yo = -xo * Tan;
    }
    // Looking directly north or south
    else
    {
      rx = g_player.pos.x;
      ry = g_player.pos.y;
      dof = 8;
    }

    while (dof < 8)
    {
      mx = (int)(rx) >> 6;
      my = (int)(ry) >> 6;
      // Our ray has hit a boundary
      if (mx >= 0 && my >= 0 && mx < MAP_TILES_X && my < MAP_TILES_Y && g_map[my][mx] == 1)
      {
        dof = 8;
        disV = cos(ra) * (rx - g_player.pos.x) - sin(ra) * (ry - g_player.pos.y);
      }
      // Check next vertical
      else
      {
        rx += xo;
        ry += yo;
        dof += 1;
      }
    }
    vx = rx;
    vy = ry;

    // HORIZONTAL
    // Check ray intersection with horizontal (east-west) wall segments
    dof = 0;
    disH = 100000;
    Tan = 1.0 / Tan;
    // Looking north(ish)
    if (sin(ra) > 0.001)
    {
      ry = (((int)g_player.pos.y >> 6) << 6) - 0.0001;
      rx = (g_player.pos.y - ry) * Tan + g_player.pos.x;
      yo = -64;
      xo = -yo * Tan;
    } 
    // Looking south(ish)
    else if (sin(ra) < -0.001)
    {
      ry = (((int)g_player.pos.y >> 6) << 6) + 64;
      rx = (g_player.pos.y - ry) * Tan + g_player.pos.x;
      yo = 64;
      xo = -yo * Tan;
    } 
    // Looking directly east or west
    else
    {
      rx = g_player.pos.x;
      ry = g_player.pos.y;
      dof = 8;
    }

    while (dof < 8)
    {
      mx = (int)(rx) >> 6;
      my = (int)(ry) >> 6;

      // Our ray has hit a boundary
      if (mx >= 0 && my >= 0 && mx < MAP_TILES_X && my < MAP_TILES_Y && g_map[my][mx] == 1)
      {
        dof = 8;
        disH = cos(ra) * (rx - g_player.pos.x) - sin(ra) * (ry - g_player.pos.y);
      } 
      // Check next horizontal
      else
      {
        rx += xo;
        ry += yo;
        dof += 1;
      } 
    }

    color = (disV < disH ? &COLOR_WALL : &COLOR_WALL_LIT);

    // Horizontal hit first
    if (disV < disH)
    {
      rx = vx;
      ry = vy;
      disH = disV;
    }

    // Draw 2D ray
    gfx_putline(g_player.pos.x, g_player.pos.y, rx, ry, *color);

    // Fix fisheye
    float ca = ang_add(g_player.ang, -ra);
    disH = disH * cos(ca);
    int lineH = (MAP_TILES_S * SCREEN_H) / (disH);
    if (lineH > SCREEN_H)
    {
      lineH = SCREEN_H;
    }
    int lineOff = (SCREEN_H / 2) - (lineH >> 1);

    // Draw a vertical slice of the wall
    gfx_putline(r + MAP_W, lineOff, r + MAP_W, lineOff + lineH, *color);
  }
}

// Check ray intersection with vertical (north-south) wall segments
// WIP
Point ray_wall_intersection_v(Point r_pos, angle r_ang) {
    int r, r_min, r_max, mx, my, mp, dof, side;
    float vx, vy, rx, ry, xo, yo;
    Point intersection;
    angle raStart, rDelta;
    SDL_Color *color;

    int dis = 100000;
    float Tan = tan(r_ang);
    
    // Looking west(ish)
    if (cos(r_ang) > 0.001)
    {
      rx = (((int)g_player.pos.x >> 6) << 6) + 64;
      ry = (g_player.pos.x - rx) * Tan + g_player.pos.y;
      xo = 64;
      yo = -xo * Tan;
    } 
    // Looking east(ish)
    else if (cos(r_ang) < -0.001)
    {
      rx = (((int)g_player.pos.x >> 6) << 6) - 0.0001;
      ry = (g_player.pos.x - rx) * Tan + g_player.pos.y;
      xo = -64;
      yo = -xo * Tan;
    }
    // Looking directly north or south
    else
    {
      rx = g_player.pos.x;
      ry = g_player.pos.y;
      dof = 8;
    }

    while (dof < 8)
    {
      mx = (int)(rx) >> 6;
      my = (int)(ry) >> 6;
      // Our ray has hit a boundary
      if (mx >= 0 && my >= 0 && mx < MAP_TILES_X && my < MAP_TILES_Y && g_map[my][mx] == 1)
      {
        dof = 8;
        dis = cos(r_ang) * (rx - g_player.pos.x) - sin(r_ang) * (ry - g_player.pos.y);
      }
      // Check next vertical
      else
      {
        rx += xo;
        ry += yo;
        dof += 1;
      }
    }
    intersection.x = rx;
    intersection.y = ry;
}