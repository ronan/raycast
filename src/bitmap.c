#include "common.h"
#include "gfx.h"
#include "bitmap.h"

Pixel g_pixel_none = COLOR_CLEAR;


void bitmap_load_all() {
  fprintf(stderr, "Loading bitmaps.\n");
  g_bitmaps[BITMAP_TEST] = SDL_LoadBMP("assets/test.bmp");
  g_bitmaps[BITMAP_WALL] = SDL_LoadBMP("assets/wall.bmp");
  g_bitmaps[BITMAP_FLOOR] = SDL_LoadBMP("assets/floor.bmp");
  g_bitmaps[BITMAP_CEILING] = SDL_LoadBMP("assets/ceiling.bmp");
  g_bitmaps[BITMAP_BLUENOISE] = SDL_LoadBMP("assets/dither/bluenoise.bmp");
  g_bitmaps[BITMAP_BAYER] = SDL_LoadBMP("assets/dither/bayer.bmp");
}

void bitmap_unload_all() {
  for (int i = 0; i < BITMAP_END; i++) {
    SDL_FreeSurface(g_bitmaps[i]);
  }
}

Pixel bitmap_sample(int bitmap_idx, Point p) {
  SDL_Surface *surface = g_bitmaps[bitmap_idx];
  SDL_PixelFormat *fmt = surface->format;

  // Convert u,v coordinates (0.0 - 1.0) to a bitmap pixel memory offset
  int px = (int)(surface->w * p.x);
  int py = (int)(surface->h * p.y);

  if (px > surface->w || py > surface->h || px < 0 || py < 0) {
    // fprintf(stderr, "Invalid bitmap index: (%d, %d). Bitmap is (%d, %d)\n", px, py, surface->w, surface->h);
  }
  else {
    SDL_LockSurface(surface);
    Uint8 pixel_idx = py * surface->pitch + px * surface->format->BytesPerPixel;
    Uint8 *pallet_idx = (Uint8 *)(surface->pixels + pixel_idx);
    SDL_Color *color = (Pixel *)&fmt->palette->colors[*pallet_idx];
    SDL_UnlockSurface(surface);
    return *color;
  }

  return g_pixel_none;
}

Pixel bitmap_sample_index(int bitmap_idx, int px, int py) {
  SDL_Surface *surface = g_bitmaps[bitmap_idx];
  SDL_PixelFormat *fmt = surface->format;

  if (px > surface->w || py > surface->h || px < 0 || py < 0) {
    return g_pixel_none;
    // fprintf(stderr, "Invalid bitmap index: (%d, %d). Bitmap is (%d, %d)\n", px, py, surface->w, surface->h);
  }
  else {
    SDL_LockSurface(surface);
    Uint8 pixel_idx = py * surface->pitch + px *surface->format->BytesPerPixel;
    Uint8 *pallet_idx = (Uint8 *)(surface->pixels + pixel_idx);
    SDL_Color *color = (Pixel *)&fmt->palette->colors[*pallet_idx];
    SDL_UnlockSurface(surface);
    return *color;
  }

  return g_pixel_none;
}

void bitmap_draw_pixel(int bitmap_idx, Point uv, int screen_x, int screen_y) {
  Pixel px = bitmap_sample(bitmap_idx, uv);
  gfx_put_pixel(screen_x, screen_y, (SDL_Color)px);
}

void bitmap_draw_to_surface(gfx_ctx gfx, int bitmap_idx, int x, int y) {
  SDL_Texture * texture = SDL_CreateTextureFromSurface(gfx.renderer, g_bitmaps[bitmap_idx]);
  SDL_RenderCopy(gfx.renderer, texture, NULL, NULL);
} 