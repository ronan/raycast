#include <sys/time.h>
#include <sys/resource.h>

#include "common.h"
#include "map.h"
#include "geometry.h"
#include "gfx.h"
#include "ray.h"
#include "player.h"
#include "scene.h"
#include "bitmap.h"
#include "critter.h"
#include "utils.h"

#include "viz.h"

gfx_ctx g_viz;

int viz_bitmap_x = VIZ_BITMAP_X;
int viz_bitmap_y = VIZ_BITMAP_Y;

float vis_stats_last_t = 0, vis_stats_t = 0, viz_stats_last_displayed = 0;
int vis_stats_frames = 0;


gfx_err viz_init(unsigned int width, unsigned int height)
{
  return gfx_init_window(&g_viz, width, height, WINDOW_W + 150, 100, "Viz");
}

void viz_destroy()
{
  SDL_DestroyRenderer(g_viz.renderer);
  SDL_DestroyWindow(g_viz.window);
}

gfx_err viz_update()
{
  SDL_RenderPresent(g_viz.renderer);
  return GFX_ERR_NONE;
}

gfx_err viz_put_line(int x1, int y1, int x2, int y2, SDL_Color color)
{
  SDL_SetRenderDrawColor(g_viz.renderer, color.r, color.g, color.b, color.a);
  SDL_RenderDrawLine(g_viz.renderer, x1, y1, x2, y2);
  return GFX_ERR_NONE;
}

gfx_err viz_put_rect(int x1, int y1, int w, int h, SDL_Color color)
{
  SDL_Rect rect;
  rect.x = x1;
  rect.y = y1;
  rect.w = w;
  rect.h = h;

  SDL_SetRenderDrawColor(g_viz.renderer, color.r, color.g, color.b, color.a);
  SDL_RenderFillRect(g_viz.renderer, &rect);
  return GFX_ERR_NONE;
}

gfx_err viz_put_dot(Point center, float size, SDL_Color color)
{
  SDL_Rect rect;
  float half_size = size/2;
  rect.x = center.x - half_size;
  rect.y = center.y - half_size;
  rect.w = size;
  rect.h = size;

  SDL_SetRenderDrawColor(g_viz.renderer, color.r, color.g, color.b, color.a);
  SDL_RenderFillRect(g_viz.renderer, &rect);
  return GFX_ERR_NONE;
}


void viz_map() {
  int x, y, xo, yo;
  SDL_Color *color;

  SDL_Color bg = COLOR_BLACK;
  SDL_SetRenderDrawColor(g_viz.renderer, bg.r, bg.g, bg.b, bg.a);
  SDL_RenderClear(g_viz.renderer);

  for (y = 0; y < MAP_TILES_Y; y++)
  {
    for (x = 0; x < MAP_TILES_X; x++)
    {
      color = (g_map[y][x] ? &COLOR_WALL : &COLOR_FLOOR);
      viz_put_rect(x * VIZ_MAP_SCALE + 1, y * VIZ_MAP_SCALE + 1, VIZ_MAP_SCALE - 1, VIZ_MAP_SCALE - 1, *color);
    }
  }

  // Draw player
  float line_l = g_player.body.radius * VIZ_MAP_SCALE;
  Point from = point_mult(g_player.body.pos, VIZ_MAP_SCALE);
  Point to = point_add(from, point_mult(g_player.body.dir, line_l));
  viz_put_line(from.x, from.y, to.x, to.y, COLOR_GREEN);
  to = point_add(from, point_mult(g_player.camera_plane, line_l));
  viz_put_line(from.x, from.y, to.x, to.y, COLOR_YELLOW);
  viz_put_dot(from, 1, COLOR_RED);

  // Draw the start/end rays for the floor scan sweep
  Point ray_dir_from = point_sub(g_player.body.dir, g_player.camera_plane);
  Point ray_dir_to = point_add(g_player.body.dir, g_player.camera_plane);
  viz_map_line(g_player.body.pos, point_add(g_player.body.pos, ray_dir_from), COLOR_YELLOW);
  viz_map_line(g_player.body.pos, point_add(g_player.body.pos, ray_dir_to), COLOR_YELLOW);


  for (int i = 0; i < 16; i++) {
    viz_put_dot(point_mult(g_lights[i], VIZ_MAP_SCALE), 5, COLOR_YELLOW);
  }

  for (int i = 0; i < MAX_CRITTERS; i++) {
    viz_put_dot(
      point_mult(g_critters[i].body.pos, VIZ_MAP_SCALE), 
      g_critters[i].body.radius * VIZ_MAP_SCALE,
      COLOR_CYAN
    );
  }

}

void viz_map_line(Point a, Point b, SDL_Color c) {
  a = point_mult(a, VIZ_MAP_SCALE);
  b = point_mult(b, VIZ_MAP_SCALE);

  // Don't draw off the map
  // if (b.x < VIZ_MAP_W) {
    viz_put_line(a.x, a.y, b.x, b.y, c);
  // }
}

void viz_map_vector(Point a, Point b, SDL_Color c) {
  a = point_mult(a, VIZ_MAP_SCALE);
  b = point_mult(b, VIZ_MAP_SCALE);
  b = point_add(a, b);
  viz_put_line(a.x, a.y, b.x, b.y, c);
}

void viz_map_dot(Point a, float size, SDL_Color c) {
  a = point_mult(a, VIZ_MAP_SCALE);
  viz_put_dot(a, size, c);
}

void viz_map_floor_ray(Ray r, SDL_Color c) {
  return;
  viz_map_dot(r.end, 1, c);
}

void viz_map_ray_critter(Ray r) {
  return;
  viz_map_line(r.start, r.end, COLOR_YELLOW);
  viz_map_dot(r.end, 3, COLOR_RED);
}

void viz_map_ray_critter_hit(Ray r) {
  return;
  viz_map_line(r.start, r.end, COLOR_YELLOW);
  viz_map_dot(r.end, 3, COLOR_CYAN);
}


void viz_map_ray(Ray r) {
  return;
  viz_map_line(r.start, r.end, COLOR_YELLOW);
  viz_map_dot(r.end, 5, COLOR_CYAN);
}

void viz_map_ray_partial(Ray r) {
  return;
  viz_map_line(r.start, r.end, COLOR_CYAN);
  viz_map_dot(r.end, 5, COLOR_RED);
}

void viz_bitmap(int bitmap_idx)
{
  SDL_Rect r = (SDL_Rect){.x = viz_bitmap_x, .y = viz_bitmap_y, .w = VIZ_BITMAP_W, .h = VIZ_BITMAP_W};
  viz_bitmap_y += VIZ_BITMAP_W;

  SDL_RenderCopy(g_viz.renderer, g_textures[bitmap_idx], NULL, &r);
}

void viz_stats() {
  // struct rusage {
  //              struct timeval ru_utime; /* user CPU time used */
  //              struct timeval ru_stime; /* system CPU time used */
  //              long   ru_maxrss;        /* maximum resident set size */
  //              long   ru_ixrss;         /* integral shared memory size */
  //              long   ru_idrss;         /* integral unshared data size */
  //              long   ru_isrss;         /* integral unshared stack size */
  //              long   ru_minflt;        /* page reclaims (soft page faults) */
  //              long   ru_majflt;        /* page faults (hard page faults) */
  //              long   ru_nswap;         /* swaps */
  //              long   ru_inblock;       /* block input operations */
  //              long   ru_oublock;       /* block output operations */
  //              long   ru_msgsnd;        /* IPC messages sent */
  //              long   ru_msgrcv;        /* IPC messages received */
  //              long   ru_nsignals;      /* signals received */
  //              long   ru_nvcsw;         /* voluntary context switches */
  //              long   ru_nivcsw;        /* involuntary context switches */
  //          };
  struct rusage snapshot;


  vis_stats_frames++;
  vis_stats_t = SDL_GetTicks();
  float viz_time_since_last_displayed = vis_stats_t - vis_stats_last_t;
  if (viz_time_since_last_displayed >= 5000.0){
    float mspf = (viz_time_since_last_displayed)/(float)vis_stats_frames;
    getrusage(0, &snapshot);

    info("********************************************");
    info("* Ticks:         %u                     ", SDL_GetTicks());
    info("* MS/Frame:     %5.2f                     ", mspf);
    info("* FPS:           %5.2f                     ", 1000/mspf);
    info("* Memory:        %5.2FMB                   ", (float)snapshot.ru_maxrss / 1048576);
    info("********************************************");
    info("");

    vis_stats_frames = 0;
    vis_stats_last_t  = vis_stats_t;
  }
}

void viz_draw() {
  viz_map();

  viz_bitmap_x = VIZ_BITMAP_X;
  viz_bitmap_y = VIZ_BITMAP_Y;
  viz_bitmap(BITMAP_BLUENOISE);
  viz_bitmap(BITMAP_BAYER);
  viz_bitmap(BITMAP_HATCH);

  viz_stats();

  // viz_update();
}