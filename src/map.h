#ifndef _MAP_H_
#define _MAP_H_

#include "common.h"

// The result when asking for a tile which is outside of the bounds of the map
#define MAP_TILE_OOB -1
#define MAP_TILE_EMPTY 0
#define MAP_TILE_WALL 1

typedef int MapTile;

typedef enum MapDir
{
    MAP_E = 0,
    MAP_N,
    MAP_W,
    MAP_S
} MapDir;

extern MapTile g_map[MAP_TILES_Y][MAP_TILES_X];
extern Point g_lights[16];

void map_init();

MapTile map_tile_at_point(Point p);
float map_align_to_tile(float d);
float map_align_point_to_tile(Point p);
int map_tile_is_oob(MapTile t);
int map_tile_is_wall(MapTile t);
int map_tile_is_empty(MapTile t);
void map_generate_maze();

#endif