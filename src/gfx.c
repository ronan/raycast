#include "common.h"
#include "gfx.h"
#include "player.h"

gfx_ctx g_gfx;

gfx_err gfx_init(unsigned int width, unsigned int height)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "Unable to initialize SDL");
        return GFX_ERR;
    }

    if (SDL_CreateWindowAndRenderer(width, height, 0, &g_gfx.window, &g_gfx.renderer) != 0)
    {
        fprintf(stderr, "Unable to initialize SDL Window or Renderer");
        return GFX_ERR;
    }

    return GFX_ERR_NONE;
}

gfx_err gfx_clear(SDL_Color color)
{
    SDL_SetRenderDrawColor(g_gfx.renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(g_gfx.renderer);
    return GFX_ERR_NONE;
}

gfx_err gfx_putpixel(unsigned int x, unsigned int y, SDL_Color color)
{
    SDL_SetRenderDrawColor(g_gfx.renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(g_gfx.renderer, x, y);
    return GFX_ERR_NONE;
}

gfx_err gfx_putline(int x1, int y1, int x2, int y2, SDL_Color color)
{
    SDL_SetRenderDrawColor(g_gfx.renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(g_gfx.renderer, x1, y1, x2, y2);
    return GFX_ERR_NONE;
}

gfx_err gfx_putrect(int x1, int y1, int w, int h, SDL_Color color)
{
    SDL_Rect rect;
    rect.x = x1;
    rect.y = y1;
    rect.w = w;
    rect.h = h;

    SDL_SetRenderDrawColor(g_gfx.renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(g_gfx.renderer, &rect);
    return GFX_ERR_NONE;
}

gfx_err gfx_update()
{
    SDL_RenderPresent(g_gfx.renderer);
    return GFX_ERR_NONE;
}

gfx_err gfx_destroy()
{
    SDL_DestroyRenderer(g_gfx.renderer);
    SDL_DestroyWindow(g_gfx.window);
    SDL_Quit();
    return GFX_ERR_NONE;
}
