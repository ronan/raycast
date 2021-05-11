#include "common.h"
#include "gfx.h"
#include "pixel.h"
#include "bitmap.h"
#include "player.h"

gfx_ctx g_gfx;

int g_num_displays = 0;
SDL_Rect g_display_bounds[MAX_DISPLAYS];

SDL_Color WALL_COLORS[4] = {
    (SDL_Color) { 200, 200, 200, 0 },
    (SDL_Color) { 180, 180, 180, 0 },
    (SDL_Color) { 100, 100, 100, 0 },
    (SDL_Color) { 90, 90, 90, 0 }
};

gfx_err gfx_init(unsigned int width, unsigned int height)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "Unable to initialize SDL z");
        return GFX_ERR;
    }

    bitmap_load_all();

    // Get the bounds of every available display
    g_num_displays = SDL_GetNumVideoDisplays();
    g_num_displays = g_num_displays <= MAX_DISPLAYS ? g_num_displays : MAX_DISPLAYS;
    for(int i = 0; i < g_num_displays; ++i)
    {
        SDL_GetDisplayBounds( i, &g_display_bounds[i] );
    }

    if (gfx_init_window(&g_gfx, width, height, 100, 100, "Game") != GFX_ERR_NONE)
    {
        fprintf(stderr, "Unable to create window");
        return GFX_ERR;
    }


    g_gfx.buffer.w = width;
    g_gfx.buffer.h = height;
    g_gfx.buffer.format = SDL_PIXELFORMAT_ARGB8888;
    g_gfx.buffer.bpp = SDL_BYTESPERPIXEL(g_gfx.buffer.format);
    g_gfx.buffer.pitch = width * g_gfx.buffer.bpp;
    g_gfx.texture = SDL_CreateTexture
        (
        g_gfx.renderer,
        g_gfx.buffer.format,
        SDL_TEXTUREACCESS_STREAMING,
        width, height
        );

    if (!g_gfx.texture) {
        fprintf(stderr, "Unable to create screen texture.");
        return GFX_ERR;
    }

    g_gfx.buffer.size = height * g_gfx.buffer.pitch;
    g_gfx.buffer.data = malloc( g_gfx.buffer.size );
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

  printf( "Could not create window or renderer. SDL Error: %s\n", SDL_GetError() );
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
    SDL_RenderCopy( g_gfx.renderer, g_gfx.texture, NULL, NULL );
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
        // fprintf(stderr, "Out of bounds pixel write. (%d, %d).\n", x, y);
        return GFX_ERR;
    }
    int offset = (g_gfx.buffer.pitch * y) + (x * g_gfx.buffer.bpp);
    g_gfx.buffer.data[ offset + 0 ] = color.b;
    g_gfx.buffer.data[ offset + 1 ] = color.g;
    g_gfx.buffer.data[ offset + 2 ] = color.r;
    g_gfx.buffer.data[ offset + 3 ] = color.a;

    return GFX_ERR_NONE;
}
