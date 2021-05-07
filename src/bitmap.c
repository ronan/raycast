#include "common.h"
#include "gfx.h"
#include "bitmap.h"

SDL_Color g_pixel_none = COLOR_CLEAR;


void bitmap_load_all() {
  fprintf(stderr, "Loading bitmaps.\n");
  g_bitmaps[BITMAP_TEST] = SDL_LoadBMP("assets/test.bmp");
  g_bitmaps[BITMAP_WALL] = SDL_LoadBMP("assets/wall.bmp");
  g_bitmaps[BITMAP_FLOOR] = SDL_LoadBMP("assets/floor.bmp");
}

void bitmap_unload_all() {
  SDL_FreeSurface(g_bitmaps[0]);
}

Pixel bitmap_sample(int bitmap_idx, Point p) {
  SDL_Surface *surface = g_bitmaps[bitmap_idx];
  SDL_PixelFormat *fmt = surface->format;
  SDL_Color *color;
  // Uint8 index;

  SDL_LockSurface(surface);

  /* Get the topleft pixel */


  int px = (int)(surface->w * p.x);
  int py = (int)(surface->h * p.y);
  int bpp = surface->format->BytesPerPixel;

  if (px > surface->w || py > surface->h || px < 0 || py < 0) {
    fprintf(stderr, "Invalid bitmap index");
    color = (SDL_Color *)&g_pixel_none;
  }
  else {
    Uint8 *index = (Uint8 *)surface->pixels + py * surface->pitch + px * bpp;
    int idx = index;
    color = &fmt->palette->colors[*index];
  }

  // int idx, r, g, b, a;
  // idx = index;
  // r = color->r;
  // g = color->g;
  // b = color->b;
  // a = color->a;



  SDL_UnlockSurface(surface);

  return (Pixel)color;
}


void bitmap_draw_pixel(int bitmap_idx, Point p, int dest_x, int dest_y) {
  Pixel pix = bitmap_sample(bitmap_idx, p);
  gfx_put_pixel(dest_x, dest_y, (SDL_Color) *pix);
}