// Draw the tiles that a ray crosses before intersecting. Kinda. Not really. Misses corners.
// This is actually just standard-ish ray casting algo with a tile sized step.
// Some of this might be useful. Looks pretty at any rate.
void ray_wall_intersection(Point r_start, angle r_ang) {
  Point v, h, v_delta, h_delta;
  float r_ang_tan = tan(r_ang);
  Point step = ang_vector(r_ang, MAP_TILES_S);

  for (h = r_start; map_tile_at_point(h) != MAP_TILE_OOB; h = point_add(h, step)) {
    float x = map_align_to_tile(h.x);
    float y = map_align_to_tile(h.y);
    if (map_tile_at_point(h)) {
      gfx_putrect(x + 2, y + 2, MAP_TILES_S - 4, MAP_TILES_S - 4, COLOR_RED);
      break;
    }
    else {
      gfx_putrect(x + 2, y + 2, MAP_TILES_S - 4, MAP_TILES_S - 4, COLOR_BLUE);
    }
  }
}