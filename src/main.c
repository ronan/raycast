#include "stdbool.h"
#include "common.h"
#include "viz.h"
#include "gfx.h"
#include "player.h"
#include "input.h"
#include "map.h"
#include "scene.h"
#include "input.h"

int main(int argc, char *argv[])
{
    fprintf(stderr, "Loading everything... \n");
    if (gfx_init(WINDOW_W, WINDOW_H))
    {
        fprintf(stderr, "SDL failed to initialize: \n%s\n", SDL_GetError());
        return -1;
    }
    viz_init(VIZ_WINDOW_W, VIZ_WINDOW_H);

    map_init();
    player_init();
    fprintf(stderr, "Finished loading\n");

    scn_draw();

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

                default:
                    break;
            }
        }
        t = SDL_GetTicks();
        float t_delta = (t - t_last);

        fps_frames++;
        float fps_time_frame = t - t_last_fps;
        if (fps_time_frame >= 5000.0){
            // printf and reset timer
            float mspf = (fps_time_frame)/(float)fps_frames;
            printf("%f ms/frame (%f fps)\n", mspf, 1000/mspf);
            fps_frames = 0;
            t_last_fps  = t;
        }

        input_scan();
        player_tick(t_delta / 33.33); // 30FPS as a standard for animation
        scn_draw();

        t_last = t;
    }

    viz_destroy();
    gfx_destroy();

    return 0;
}