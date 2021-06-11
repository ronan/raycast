#include "common.h"
#include "geometry.h"
#include "viz.h"
#include "utils.h"

#include "map.h"

MapTile g_map[MAP_TILES_Y][MAP_TILES_X]; 
Point g_lights[16];

Point direction_offsets[4] = {
  (Point){1,0},
  (Point){0,-1},
  (Point){-1,0},
  (Point){0,1}
};

//the map array. Edit to change level but keep the outer walls
int map_sample[MAP_TILES_Y][MAP_TILES_X] =
{
 {1,1,1,1,1,1,1,1,1},
 {1,0,1,0,1,0,0,0,1},
 {1,0,1,0,1,1,1,0,1},
 {1,0,1,0,0,0,1,0,1},
 {1,0,0,0,0,0,1,0,1},
 {1,0,0,0,0,0,0,0,1},
 {1,0,0,0,0,1,0,0,1},
 {1,0,0,0,0,0,0,0,1},
 {1,1,1,1,1,1,1,1,1},
};

int empty_map[MAP_TILES_Y][MAP_TILES_X]=
{
 {1,1,1,1,1,1,1,1,1},
 {1,0,0,0,0,0,0,0,1},
 {1,0,0,0,0,0,0,0,1},
 {1,0,0,0,0,0,0,0,1},
 {1,0,0,0,0,0,0,0,1},
 {1,0,0,0,0,0,0,0,1},
 {1,0,0,0,0,0,0,0,1},
 {1,0,0,0,0,0,0,0,1},
 {1,1,1,1,1,1,1,1,1},
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

int map_tile_is_empty(MapTile t) {
  return t == MAP_TILE_EMPTY;
}

// Round a position (x or y) down to the nearest tile boundary
float map_align_to_tile(float d) {
  return ((int)(d) / MAP_TILES_S) * MAP_TILES_S;
}

void map_fill() {
  for (int i = 0; i < MAP_TILES_Y; i++) {
    for (int j = 0; j < MAP_TILES_X; j++) {
      g_map[i][j] = MAP_TILE_WALL;
    }
  }
}

int map_is_dir_visited(Point p, MapDir dir) {
  Point offset = (Point){0,0};
  switch (dir) {
    case MAP_N:
      offset.y = -1;
      break;
    case MAP_S:
      offset.y = 1;
      break;
    case MAP_E:
      offset.x = 1;
      break;
    case MAP_W:
      offset.x = -1;
      break;
  }
  
  return map_tile_is_wall(map_tile_at_point(point_add(p, offset))) &&
          map_tile_is_wall(map_tile_at_point(point_add(p, point_mult(offset, 2))));
}

void map_generate_maze() {
  int visited = 1;
  int stack_l = 255;
  Point stack[stack_l];
  int neighbors[4];
  int neighbors_idx = -1;
  int stack_idx = -1;
  int cell_size = 2;
  
  map_fill();

  stack[++stack_idx] = (Point){1,1};
  g_map[(int)stack[stack_idx].y][(int)stack[stack_idx].x] = MAP_TILE_EMPTY;
  
  int num_cells = ((MAP_TILES_X / cell_size) * (MAP_TILES_Y / cell_size));

  while (visited < num_cells) {
    // viz_draw();

    // Make a list of visited neighbors
    neighbors_idx = -1;
    if (map_is_dir_visited(stack[stack_idx], MAP_N)) {
      neighbors[++neighbors_idx] = MAP_N;
    }
    if (map_is_dir_visited(stack[stack_idx], MAP_S)) {
      neighbors[++neighbors_idx] = MAP_S;
    }
    if (map_is_dir_visited(stack[stack_idx], MAP_E)) {
      neighbors[++neighbors_idx] = MAP_E;
    }
    if (map_is_dir_visited(stack[stack_idx], MAP_W)) {
      neighbors[++neighbors_idx] = MAP_W;
    }

    if (neighbors_idx >= 0) {
      MapDir next = neighbors[rand_int(neighbors_idx+1)];
      Point offset = direction_offsets[next];
      Point next_point = point_add(stack[stack_idx], offset);
      g_map[(int)next_point.y][(int)next_point.x] = MAP_TILE_EMPTY;
      next_point = point_add(next_point, offset);
      g_map[(int)next_point.y][(int)next_point.x] = MAP_TILE_EMPTY;
      stack[++stack_idx] = next_point;
      visited++;
    }
    else {
      stack_idx--;
    }

  }

}


void map_init() {
  int idx = 0;
  // for (int i = 1; i < MAP_TILES_Y; i +=2) {
  //   for (int j = 1; j < MAP_TILES_X; j += 2) {
  //     g_lights[idx++] = (Point){j + .5, i + .5};
  //   }
  // }

  // map_generate_maze();

  for (int y = 0; y < MAP_TILES_Y; y++) {
    for (int x = 0; x < MAP_TILES_X; x++) {
      g_map[y][x] = map_generate_tile(empty_map[y][x]);
    }
  }
}