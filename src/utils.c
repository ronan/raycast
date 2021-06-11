#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


float rand_unit() {
  return (rand() / (float)RAND_MAX);
}

float rand_scaled(float scale) {
  return rand_unit() * scale;
}

float rand_range(float min, float max) {
  return min + rand_scaled(max - min);
}

float rand_perturb(float base, float scale) {
  return base - scale + rand_scaled(2 * scale);
}

int rand_int(int max) {
  return rand() % max;
}

int clamp_int(int i, int min, int max) {
  i = i < min ? min : i;
  i = i > max ? max : i;
  return i;
}

void err(char const *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, fmt, args);
  va_end(args);  
  char logmsg;
}

void fatal(char const *fmt, ...)
{
    char buffer[256];

    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    fputs(buffer, stderr);
    fputs("\n", stderr);
    va_end(args);
}

void info(const char *fmt, ...) {
    char buffer[256];

    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    fputs(buffer, stderr);
    fputs("\n", stderr);
    va_end(args);
}