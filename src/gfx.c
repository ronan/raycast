
#include "common.h"
#include "pixel.h"
#include "bitmap.h"
#include "player.h"
#include "utils.h"

#include "gfx.h"

gfx_ctx g_gfx;

int g_num_displays = 0;
SDL_Rect g_display_bounds[MAX_DISPLAYS];

SDL_Color WALL_COLORS[4] = {
    (SDL_Color) { 200, 200, 200, 255 },
    (SDL_Color) { 180, 180, 180, 255 },
    (SDL_Color) { 100, 100, 100, 255 },
    (SDL_Color) { 90, 90, 90, 255 }
};

gfx_err gfx_init()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        err("Unable to initialize SDL");
        return GFX_ERR;
    }

    // Get the bounds of every available display
    g_num_displays = SDL_GetNumVideoDisplays();
    g_num_displays = g_num_displays <= MAX_DISPLAYS ? g_num_displays : MAX_DISPLAYS;
    for(int i = 0; i < g_num_displays; ++i)
    {
        SDL_GetDisplayBounds( i, &g_display_bounds[i] );
    }

    if (gfx_init_window(&g_gfx, WINDOW_W, WINDOW_H, 100, 100, "Game") != GFX_ERR_NONE)
    {
        err("Unable to create window");
        return GFX_ERR;
    }

    int imgFlags = IMG_INIT_PNG;
    if( !( IMG_Init( imgFlags ) & imgFlags ) )
    {
        err( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
        return GFX_ERR;
    }

    g_gfx.buffer.w = SCREEN_W;
    g_gfx.buffer.h = SCREEN_H;
    g_gfx.buffer.format = SDL_PIXELFORMAT_ARGB8888;
    g_gfx.buffer.bpp = SDL_BYTESPERPIXEL(g_gfx.buffer.format);
    g_gfx.buffer.pitch = SCREEN_W * g_gfx.buffer.bpp;
    g_gfx.buffer.size = SCREEN_H * g_gfx.buffer.pitch;
    g_gfx.buffer.data = malloc( g_gfx.buffer.size );

    g_gfx.texture = SDL_CreateTexture
        (
        g_gfx.renderer,
        g_gfx.buffer.format,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_W, SCREEN_H
        );

    if (!g_gfx.texture) {
        err("Unable to create screen texture.");
        return GFX_ERR;
    }

    return GFX_ERR_NONE;
}

gfx_err gfx_init_window(gfx_ctx *ctx, int width, int height, int x, int y, const char *title)
{
  ctx->window = SDL_CreateWindow(title, x, y, width, height, SDL_WINDOW_SHOWN);
  if (ctx->window) {

    // Move window to second display
    int display = g_num_displays - 1;
    SDL_SetWindowPosition( ctx->window, g_display_bounds[display].x + x, g_display_bounds[display].y + y );

    ctx->renderer = SDL_CreateRenderer(ctx->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ctx->renderer) {
      return GFX_ERR_NONE;
    }
  }

  err("Could not create window or renderer. SDL Error: %s\n", SDL_GetError());
  return GFX_ERR;
}


gfx_err gfx_destroy()
{
    free(g_gfx.buffer.data);

    SDL_DestroyRenderer(g_gfx.renderer);
    SDL_DestroyWindow(g_gfx.window);
    
    SDL_Quit();

    bitmap_unload_all();

    return GFX_ERR_NONE;
}


gfx_err gfx_update()
{
    SDL_UpdateTexture
    (
        g_gfx.texture,
        NULL,
        g_gfx.buffer.data,
        g_gfx.buffer.pitch
    );
    SDL_RenderCopy( g_gfx.renderer, g_gfx.texture, NULL, NULL);
    SDL_RenderPresent(g_gfx.renderer);
    return GFX_ERR_NONE;
}


gfx_err gfx_clear(SDL_Color color)
{
    SDL_SetRenderDrawColor(g_gfx.renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(g_gfx.renderer);
    return GFX_ERR_NONE;
}

gfx_err gfx_put_pixel(unsigned int x, unsigned int y, Pixel color)
{
    if (x < 0 || y < 0 || x >= g_gfx.buffer.w || y >= g_gfx.buffer.h) {
        err("Out of bounds pixel write. (%d, %d).\n", x, y);
        return GFX_ERR;
    }
    int offset = (g_gfx.buffer.pitch * y) + (x * g_gfx.buffer.bpp);
    if (color.a < 128) {
        return GFX_ERR_NONE;
        // Pixel bufferPixel = (Pixel) {
        //     g_gfx.buffer.data[ offset + 2 ],
        //     g_gfx.buffer.data[ offset + 1 ],
        //     g_gfx.buffer.data[ offset + 0 ],
        //     255
        // };
        // color = pixel_blend(bufferPixel, color, color.a / 255.0);
    }
    g_gfx.buffer.data[ offset + 0 ] = color.b;
    g_gfx.buffer.data[ offset + 1 ] = color.g;
    g_gfx.buffer.data[ offset + 2 ] = color.r;
    g_gfx.buffer.data[ offset + 3 ] = 255;

    return GFX_ERR_NONE;
}
