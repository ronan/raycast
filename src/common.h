#ifndef _COMMON_H_
#define _COMMON_H_

#include "gfx.h"

extern gfx_ctx g_gfx;

#define PLAYER_TURN_ANG 5 * M_PI / 180.0
#define PLAYER_SPEED .1

#define SCREEN_W 640
#define SCREEN_H 480

#define SCREEN_HORIZON 240

#define SCREEN_X 0
#define SCREEN_Y 0

#define FOCAL_LENGTH 0.6

#define WINDOW_W SCREEN_W
#define WINDOW_H SCREEN_H

#define MAP_TILES_Y 8
#define MAP_TILES_X 8
#define MAP_TILES_S 1 //map cube size

#define MINIMAP_SCALE 64

#define WALL_H 1.0


#endif