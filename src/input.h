#ifndef _CTL_H_
#define _CTL_H_


typedef struct InputState {
  int rotate_l;
  int rotate_r;
  int move_f;
  int move_b;
} InputState;

extern InputState g_input;

void input_scan();
void input_handlekey(SDL_Keysym key);

#endif