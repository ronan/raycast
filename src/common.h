#ifndef _COMMON_H_
#define _COMMON_H_

#include "gfx.h"

extern gfx_ctx g_gfx;

#define PLAYER_TURN_ANG 5 * M_PI / 180.0
#define PLAYER_SPEED .1

#define SCREEN_W 128
#define SCREEN_H 128

#define SCREEN_SCALE 4

#define WINDOW_W SCREEN_W * SCREEN_SCALE
#define WINDOW_H SCREEN_H * SCREEN_SCALE

#define SCREEN_HORIZON (SCREEN_H/2)

#define SCREEN_X 0
#define SCREEN_Y 0

#define FOCAL_LENGTH 0.6
#define CAMERA_HEIGHT (float)SCREEN_HORIZON

#define WALL_H FOCAL_LENGTH * SCREEN_W / SCREEN_H 

#define MAP_TILES_Y 9
#define MAP_TILES_X 9
#define MAP_TILES_S 1 //map cube size

#define MINIMAP_SCALE 64



#endif