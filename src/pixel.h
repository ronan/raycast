#ifndef _PIXEL_H_
#define _PIXEL_H_

#include <SDL2/SDL.h>

typedef SDL_Color Pixel;

Pixel pixel_darken(Pixel p, float amount);

#endif