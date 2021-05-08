#include <stdint.h>

#include "pixel.h"

Pixel pixel_darken(Pixel p, float amount) {
  if (amount > 1) return p;

  p.r = (u_int8_t)((float)p.r * amount);
  p.g = (u_int8_t)((float)p.g * amount);
  p.b = (u_int8_t)((float)p.b * amount);

  return p;
}