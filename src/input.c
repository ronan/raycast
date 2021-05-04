#include "common.h"
#include "input.h"
#include "player.h"

void input_handlekey(SDL_Keysym key)
{
  fprintf(stderr, "Key Detected: %c", key.sym);
  switch (key.sym)
  {
  case SDLK_LEFT:
  case 'a':
    player_mv_left();
    break;
  case SDLK_RIGHT:
  case 'd':
    player_mv_right();
    break;
  case SDLK_UP:
  case 'w':
    player_mv_up();
    break;
  case SDLK_DOWN:
  case 's':
    player_mv_down();
    break;
  }
}