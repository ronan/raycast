#ifndef _COMMON_H_
#define _COMMON_H_

#include "gfx.h"

extern gfx_ctx g_gfx;

#define FOV 80.0 * M_PI / 180.0
#define PLAYER_TURN_ANG 1 * M_PI / 180.0
#define PLAYER_SPEED 5.0

#define SCREEN_W 1024
#define SCREEN_H 512
#define MAP_W 512
#define WINDOW_W MAP_W + SCREEN_W
#define WINDOW_H 512

#define MAP_TILES_Y 8
#define MAP_TILES_X 8
#define MAP_TILES_S 64 //map cube size

#endif