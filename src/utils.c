#include <stdarg.h>
#include <stdio.h>


void err(char const *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  // fprintf(stderr, fmt, args);
  va_end(args);  char logmsg;
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