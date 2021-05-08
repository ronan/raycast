#ifndef _GFX_H_
#define _GFX_H_

#include <SDL2/SDL.h>

#include "geometry.h"

typedef struct Pixel_Buffer
{
    int size;
    int pitch;
    int bpp;
    int w;
    int h;
    Uint32 format;
    Uint8 *data;
} Pixel_Buffer;

typedef struct gfx_ctx
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    Pixel_Buffer buffer;
} gfx_ctx;

typedef enum gfx_err
{
    GFX_ERR_NONE,
    GFX_ERR
} gfx_err;

gfx_err gfx_init(unsigned int width, unsigned int height);
gfx_err gfx_init_window(gfx_ctx *ctx, int width, int height, int x, int y, const char *title);
gfx_err gfx_clear(SDL_Color);
gfx_err gfx_put_pixel(unsigned int, unsigned int, SDL_Color);
gfx_err gfx_put_line(int x1, int y1, int x2, int y2, SDL_Color color);
gfx_err gfx_put_rect(int x1, int y1, int x2, int y2, SDL_Color color);
gfx_err gfx_put_square_centered(Point center, float size, SDL_Color color);
gfx_err gfx_update();
gfx_err gfx_destroy();
gfx_err gfx_draw_screen();

#define COLOR_BLACK \
    (SDL_Color) { 0, 0, 0, 0 }
#define COLOR_RED \
    (SDL_Color) { 255, 0, 0, 200 }
#define COLOR_GREEN \
    (SDL_Color) { 0, 255, 0, 0 }
#define COLOR_BLUE \
    (SDL_Color) { 0, 0, 255, 0 }
#define COLOR_YELLOW \
    (SDL_Color) { 255, 255, 0, 0 }
#define COLOR_CYAN \
    (SDL_Color) { 255, 255, 0, 0 }
#define COLOR_MAGENTA \
    (SDL_Color) { 255, 255, 0, 0 }
#define COLOR_CLEAR \
    (SDL_Color) { 0, 0, 0, 255 }

#define MAX_DISPLAYS 2


extern SDL_Color WALL_COLORS[4];

#endif