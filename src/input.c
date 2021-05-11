#include "common.h"
#include "input.h"
#include "player.h"

InputState g_input;

void input_scan() {
    //Set texture based on current keystate
    const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );

    g_input.move_f = currentKeyStates[ SDL_SCANCODE_UP ];
    g_input.move_b = currentKeyStates[ SDL_SCANCODE_DOWN ];
    g_input.rotate_l = currentKeyStates[ SDL_SCANCODE_LEFT ];
    g_input.rotate_r = currentKeyStates[ SDL_SCANCODE_RIGHT ];

    g_input.dither = !currentKeyStates[ SDL_SCANCODE_E ];
}
