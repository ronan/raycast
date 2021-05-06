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
      gfx_put_rect(x + 2, y + 2, MAP_TILES_S - 4, MAP_TILES_S - 4, COLOR_RED);
      break;
    }
    else {
      gfx_put_rect(x + 2, y + 2, MAP_TILES_S - 4, MAP_TILES_S - 4, COLOR_BLUE);
    }
  }
}


Point ray_find_first_intersection(Point start, Point step) {
  Point out = start;
   
  // Step from tile edge to tile edge until we find a wall
  // TODO: Be more clever than simply checking 8 times.
  for (int i = 0; i < 8; i++, out = point_add(out, step))
  {
    // gfx_put_square_centered(out, 3, COLOR_RED);
    MapTile t = map_tile_at_point(out);

    // Our ray has left the map
    if (map_tile_is_oob(t))
    {
      return out;
    }

    // Our ray has hit a boundary
    if (map_tile_is_wall(t))
    {
      return out;
    }
  }

  // Shouldn't get here in a closed map. Not sure what happens next
  // Probably want to deal with this in the renderer by having an OOB check and drawing nothing
  return POINT_OOB;
}

// Check ray intersection with horizontal (east-west) wall segments
Point ray_wall_intersection_h(Point r_pos, angle r_ang) {
  Point out, step;

  float r_ang_tan = (1.0 / tan(r_ang));

  // Looking north(ish) seach in the negative y direction
  if (ang_is_northward(r_ang))
  {
    out.y = map_align_to_tile(r_pos.y) - 0.0001;
    step.y = -MAP_TILES_S;
  }
  else {
    out.y = map_align_to_tile(r_pos.y) + MAP_TILES_S;
    step.y = MAP_TILES_S;
  }

  // Derive x using tan(r) to find the distance from the initial position
  out.x = r_pos.x - ((r_pos.y - out.y) * -r_ang_tan);
  step.x = -step.y * r_ang_tan;

  return ray_find_first_intersection(out, step);
}

// Check ray intersection with vertical (north-south) wall segments
Point ray_wall_intersection_v(Point r_pos, angle r_ang) {
  Point out, step;

  // I don't know why I have to rotate the angle by 90˚ to make this work
  float r_ang_tan = (1.0 / tan(r_ang - (M_PI/2)));

  // Looking westward seach in the negative x direction
  if (ang_is_westward(r_ang))
  {
    out.x = map_align_to_tile(r_pos.x) - 0.0001;
    step.x = -MAP_TILES_S;
  }
  else {
    out.x = map_align_to_tile(r_pos.x) + MAP_TILES_S;
    step.x = MAP_TILES_S;
  }

  // Derive y using tan(r) to find the distance from the initial position
  out.y = r_pos.y - ((r_pos.x - out.x) * r_ang_tan);
  step.y = step.x * r_ang_tan;

  return ray_find_first_intersection(out, step);
}

// point, angle, range
void ray_draw(int r, angle r_ang) {
  SDL_Color *color;
  float distance, dis_v = DAMN_NEAR_INFINITY, dis_h = DAMN_NEAR_INFINITY;
  Point intersection, int_v, int_h;

  int_v = ray_wall_intersection_v(g_player.pos, r_ang);
  int_h = ray_wall_intersection_h(g_player.pos, r_ang);

  dis_v = cos(r_ang) * (int_v.x - g_player.pos.x) - sin(r_ang) * (int_v.y - g_player.pos.y);
  dis_h = cos(r_ang) * (int_h.x - g_player.pos.x) - sin(r_ang) * (int_h.y - g_player.pos.y);

  if (dis_v < dis_h) {
    color = &COLOR_WALL;
    distance = dis_v;
    intersection = int_v;
  }
  else {
    color = &COLOR_WALL_LIT;
    distance = dis_h;
    intersection = int_h;
  }

  // Fix fisheye
  float ca = ang_add(g_player.ang, -r_ang);
  distance = distance * cos(ca);

  // Draw 2D ray DEBUG
  // gfx_put_line(g_player.pos.x, g_player.pos.y, intersection.x, intersection.y, *color);
  gfx_put_line(g_player.pos.x, g_player.pos.y, intersection.x, intersection.y, *color);
 
  Point v = ang_vector(r_ang, distance);
  gfx_put_line(g_player.pos.x, g_player.pos.y, g_player.pos.x + v.x, g_player.pos.y + v.y, COLOR_RED);

  // Draw intersection points DEBUG
  // gfx_put_square_centered(int_v, 10, COLOR_BLUE);
  // gfx_put_square_centered(int_h, 10, COLOR_BLUE);
  // gfx_put_square_centered(intersection, 10, COLOR_GREEN);

  // Draw a vertical slice of the wall
  int line_h = (WALL_H * SCREEN_H) / (distance);
  int line_top = (SCREEN_H / 2) - (line_h / 2);
  gfx_put_line(SCREEN_X + r, SCREEN_Y + line_top, SCREEN_X + r, SCREEN_Y + line_top + line_h, *color);
}

