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
#include "utils.h"

int main(int argc, char *argv[])
{
    info("Loading everything... \n");
    if (gfx_init())
    {
        err("SDL failed to initialize: \n%s\n", SDL_GetError());
        return -1;
    }
    viz_init(VIZ_WINDOW_W, VIZ_WINDOW_H);
    bitmap_load_all();
    map_init();
    player_init();
    info("Finished loading\n");

    bool done;
    SDL_Event event;

    float t, t_last, t_last_fps = SDL_GetTicks();
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
                            bitmap_unload_all();
                            bitmap_load_all();
                        break;
                    }
                    break;

                default:
                    break;
            }
        }
        t = SDL_GetTicks();
        float t_delta = (t - t_last);

        input_scan();
        player_tick(t_delta / 33.33); // 30FPS as a standard for animation
        scn_draw();

        t_last = t;
    }

    viz_destroy();
    gfx_destroy();

    return 0;
}