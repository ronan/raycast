#ifndef _COMMON_H_
#define _COMMON_H_

#include "gfx.h"

extern gfx_ctx g_gfx;

extern int g_paused;


#define DAMN_NEAR_INFINITY 10000
#define SOME_TINY_AMOUNT  0.00001
#define AAAAAAALMOST_ONE 0.9999



#define PLAYER_TURN_ANG 5 * M_PI / 180.0
#define PLAYER_SPEED .1



#define SCREEN_W 256
#define SCREEN_H 128
#define WINDOW_W 1024
#define WINDOW_H 512
#define SCREEN_SCALE_W (WINDOW_W / SCREEN_W)
#define SCREEN_SCALE_H (WINDOW_H / SCREEN_H)
#define SCREEN_RATIO ((float)WINDOW_W / (float)WINDOW_H)
#define SCREEN_HORIZON (SCREEN_H/2)
#define SCREEN_X 0
#define SCREEN_Y 0


#define CAMERA_HEIGHT 0.5
#define FOV 0.7

#define WALL_H (1.0 / SCREEN_H)

#define SCREEN_ROOM_H (WALL_H * SCREEN_H)

#define MAP_CELLS_X 4
#define MAP_CELLS_Y 4
#define MAP_TILES_Y (MAP_CELLS_Y * 2 + 1)
#define MAP_TILES_X (MAP_CELLS_Y * 2 + 1)
#define MAP_TILES_S 1 //map cube size
#define MINIMAP_SCALE 32



#define MAX_CRITTERS 32
#define CRITTER_MAX_TURN 0.2
// #define CRITTER_MAX_SPEED 0
#define CRITTER_MAX_SPEED 0.05
#define CRITTER_RADIUS 0.5



#endif