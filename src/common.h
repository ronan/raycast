#ifndef _COMMON_H_
#define _COMMON_H_

#include "gfx.h"

extern gfx_ctx g_gfx;

#define FOCAL_LENGTH 0.6
#define PLAYER_TURN_ANG 5 * M_PI / 180.0
#define PLAYER_SPEED .1

#define SCREEN_W 1024
#define SCREEN_W_HALF 512
#define SCREEN_H 512

#define MAP_WIDTH 512
#define SCREEN_X MAP_WIDTH
#define SCREEN_Y 0
#define WINDOW_W MAP_WIDTH + SCREEN_W
#define WINDOW_H 512

#define MAP_TILES_Y 8
#define MAP_TILES_X 8
#define MAP_TILES_S 1 //map cube size

#define MINIMAP_SCALE 64


#endif