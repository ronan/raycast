#include "common.h"
#include "geometry.h"
#include "bitmap.h"
#include "pixel.h"
#include "scene.h"
#include "map.h"
#include "gfx.h"
#include "critter.h"
#include "viz.h"
#include "ray.h"

#include "render.h"


Pixel render_critters(Ray *r, Pixel c) {
  int critter = -1;

  for (int i = 0; i < MAX_CRITTERS; i++) {
    int hit = ray_billboard_intersection(r, g_critters[i].body.pos, g_critters[i].body.radius);
    if (hit) {
      critter = i;
      viz_map_ray_critter_hit(*r);
    }
    else
    {
      viz_map_ray_critter(*r);
    }
  }
  if (critter >= 0) {
    c = bitmap_sample(BITMAP_CRITTER, r->hit.local);
  }
  return c;
}

Pixel render_lights(Ray r, Pixel c) {
  float intensity = .1;

  // Ambient
  float brightness = .1;

  for (int i = 0; i < 16; i++) {
    Point diff = point_sub(r.end, g_lights[i]);
    float dist = diff.x * diff.x + diff.y * diff.y;

    brightness += (1.0/dist)*intensity;
  }

  if (brightness > .999) {
    return c;
  }
  return pixel_darken(c, 1.0 - brightness);
}

Pixel render_floor(Ray r) {
  // Get the fractional part of the position to determine the texture coordinates
  Pixel p = bitmap_sample(BITMAP_FLOOR, r.hit.local);
  p = render_critters(&r, p);
  p = render_lights(r, p);
  return p;
}

Pixel render_ceiling(Ray r) {
  Pixel p = bitmap_sample(BITMAP_CEILING, r.hit.local);
p = render_critters(&r, p);
  p = render_lights(r, p);
  return p;
}

// pos_local is the position in coordinates on the wall (x, z) or (y, z). Range 0.0-1.0
Pixel render_wall(Ray r) {
  Pixel p = bitmap_sample(BITMAP_WALL, r.hit.local);
  p = render_critters(&r, p);
  p = render_lights(r, p);
  return p;
}
