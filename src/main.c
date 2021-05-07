#include "stdbool.h"
#include "common.h"
#include "gfx.h"
#include "player.h"
#include "input.h"
#include "map.h"
#include "scene.h"

int main(int argc, char *argv[])
{
    fprintf(stderr, "Loading Game");
    if (gfx_init(WINDOW_W, WINDOW_H))
    {
        fprintf(stderr, "SDL failed to initialize: \n%s\n", SDL_GetError());
        return -1;
    }

    map_init();
    player_init();
    fprintf(stderr, "Finished Loading Game");

    scn_draw();

    bool done;
    SDL_Event event;

    while ((!done) && (SDL_WaitEvent(&event)))
    {
        switch (event.type)
        {
        case SDL_USEREVENT:
            // HandleUserEvents(&event);
            break;

        case SDL_KEYDOWN:
            // case SDL_KEYUP:
            input_handlekey(event.key.keysym);
            scn_draw();
            break;

        case SDL_MOUSEBUTTONDOWN:
            break;

        case SDL_QUIT:
            done = true;
            break;

        default:
            break;
        } // End switch

    } // End while

    gfx_destroy();

    return 0;
}