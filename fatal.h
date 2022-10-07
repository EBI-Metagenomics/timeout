#ifndef FATAL_H
#define FATAL_H

#include <stdnoreturn.h>

void noreturn fatalxc(int excode, char const *fmt, ...);
#define fatal(...) fatalxc(EXIT_FAILURE, __VA_ARGS__)
void noreturn pfatalxc(int excode, char const *fmt, ...);
#define pfatal(...) pfatalxc(EXIT_FAILURE, __VA_ARGS__)

#endif
