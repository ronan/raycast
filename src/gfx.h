#ifndef _GFX_H_
#define _GFX_H_

#include <SDL2/SDL.h>

typedef struct gfx_ctx
{
    SDL_Window *window;
    SDL_Renderer *renderer;
} gfx_ctx;

typedef enum gfx_err
{
    GFX_ERR_NONE,
    GFX_ERR
} gfx_err;

gfx_err gfx_init(unsigned int width, unsigned int height);
gfx_err gfx_clear(SDL_Color);
gfx_err gfx_putpixel(unsigned int, unsigned int, SDL_Color);
gfx_err gfx_putline(int x1, int y1, int x2, int y2, SDL_Color color);
gfx_err gfx_putrect(int x1, int y1, int x2, int y2, SDL_Color color);
gfx_err gfx_update();
gfx_err gfx_destroy();
gfx_err gfx_draw_screen();

#define COLOR_BLACK \
    (SDL_Color) { 0, 0, 0, 0 }
#define COLOR_RED \
    (SDL_Color) { 255, 0, 0, 0 }
#define COLOR_GREEN \
    (SDL_Color) { 0, 255, 0, 0 }
#define COLOR_BLUE \
    (SDL_Color) { 0, 0, 255, 0 }

#endif