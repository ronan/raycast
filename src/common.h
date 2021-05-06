#ifndef _COMMON_H_
#define _COMMON_H_

#include "gfx.h"

extern gfx_ctx g_gfx;

#define FOCAL_LENGTH 0.8
#define PLAYER_TURN_ANG 1 * M_PI / 180.0
#define PLAYER_SPEED 5.0

#define SCREEN_W 1024
#define SCREEN_H 512

#define MAP_WIDTH 512
#define SCREEN_X MAP_WIDTH
#define SCREEN_Y 0
#define WINDOW_W MAP_WIDTH + SCREEN_W
#define WINDOW_H 512

#define MAP_TILES_Y 8
#define MAP_TILES_X 8
#define MAP_TILES_S 64 //map cube size


#endif