#include "common.h"
#include "geometry.h"

#include "map.h"

MapTile g_map[8][8];

//the map array. Edit to change level but keep the outer walls
int map_sample[MAP_TILES_Y][MAP_TILES_X] =
{
 {1,1,1,1,1,1,1,1},
 {1,0,1,0,0,0,0,1},
 {1,0,1,0,0,0,0,1},
 {1,0,1,0,0,0,0,1},
 {1,0,0,0,0,0,0,1},
 {1,0,0,0,0,1,0,1},
 {1,0,0,0,0,0,0,1},
 {1,1,1,1,1,1,1,1},
};

int empty_map[8][8]=
{
 {1,1,1,1,1,1,1,1},
 {1,0,0,0,0,0,0,1},
 {1,0,0,0,0,0,0,1},
 {1,0,0,0,0,0,0,1},
 {1,0,0,0,0,0,0,1},
 {1,0,0,0,0,0,0,1},
 {1,0,0,0,0,0,0,1},
 {1,1,1,1,1,1,1,1},
};


MapTile map_generate_tile(int value) {
  return (MapTile) value;
}


// Get the map tile that is at the given point
MapTile map_tile_at_point(Point p) {
  int mx = floor(p.x);
  int my = floor(p.y);

  if (mx >= 0 && my >= 0 && mx < MAP_TILES_X && my < MAP_TILES_Y) {
    return g_map[my][mx];
  }

  return MAP_TILE_OOB;
}

int map_tile_is_oob(MapTile t) {
  return t == MAP_TILE_OOB;
}

int map_tile_is_wall(MapTile t) {
  return t == MAP_TILE_WALL;
}

// Round a position (x or y) down to the nearest tile boundary
float map_align_to_tile(float d) {
  return ((int)(d) / MAP_TILES_S) * MAP_TILES_S;
}


void map_init() {
  for (int y = 0; y < MAP_TILES_Y; y++) {
    for (int x = 0; x < MAP_TILES_X; x++) {
      g_map[y][x] = map_generate_tile(empty_map[y][x]);
    }
  }
}