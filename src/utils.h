#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdarg.h>


void err(char const *fmt, ...);
void info(char const *fmt, ...);


float rand_unit();
int rand_int(int max);
float rand_scaled(float scale);
float rand_range(float min, float max);
float rand_perturb(float base, float scale);
int clamp_int(int i, int min, int max);
#endif