#ifndef _PIXEL_H_
#define _PIXEL_H_

#include <SDL2/SDL.h>

typedef SDL_Color Pixel;

Pixel pixel_darken(Pixel p, float amount);
Pixel pixel_lerp_linear(Pixel a, Pixel b, float t);
Pixel pixel_blend(Pixel a, Pixel b);
Pixel pixel_overlay(Pixel a, Pixel b);

#endif