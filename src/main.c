#include "stdbool.h"
#include "common.h"
#include "viz.h"
#include "gfx.h"
#include "bitmap.h"
#include "player.h"
#include "input.h"
#include "map.h"
#include "scene.h"
#include "input.h"
#include "critter.h"
#include "particle.h"
#include "utils.h"
#include "map.h"


int g_paused = 0;
Uint32 g_tick = 0;

void init()
{
    info("Initializing game");
    srand(1);

    viz_init(VIZ_WINDOW_W, VIZ_WINDOW_H);
    map_init();
    player_init();

    bitmap_load_all();
    critters_init();
    particles_init();
    info("Finished initializing\n");
}

void reinit() {
    particles_destroy();
    particles_init();
    bitmap_unload_all();
    bitmap_load_all();
    critters_init();
    critters_destroy();
}

void destroy()
{
    critters_destroy();
    particles_destroy();
    bitmap_unload_all();

    viz_destroy();
    gfx_destroy();
}

int main(int argc, char *argv[])
{

    info("Loading everything... \n");
    if (gfx_init())
    {
        err("SDL failed to initialize: \n%s\n", SDL_GetError());
        return -1;
    }

    init();

    bool done = 0;
    SDL_Event event;

    g_tick = SDL_GetTicks();
    float t, t_last, t_delta, t_last_fps = SDL_GetTicks();
    int fps_frames = 0;

    while(!done)
    {
        //Handle events on queue
        while(SDL_PollEvent(&event) != 0)
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    done = true;
                    break;

                case SDL_KEYDOWN:
                    //Select surfaces based on key press
                    switch( event.key.keysym.sym )
                    {
                        case SDLK_r:
                            reinit();
                        break;
                        case SDLK_m:
                            map_generate_maze();
                        break;
                        case SDLK_p:
                            g_paused = !g_paused;
                        break;
                    }
                    break;

                default:
                    break;
            }
        }
        t = SDL_GetTicks();
        t_delta = (t - t_last);

        // Don't let the delta get to big or collision detection gets hard
        t_delta = t_delta > 500 ? 500 : t_delta;

        input_scan();
        player_tick(t_delta / 33.33); // 30FPS as a standard for animation
        if (!g_paused) {
            critters_tick(t_delta / 33.33);
            particles_tick(t_delta / 33.33);
        }
        scn_draw();

        t_last = t;
    }

    destroy();

    return 0;
}