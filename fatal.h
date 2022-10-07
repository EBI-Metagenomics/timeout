#ifndef FATAL_H
#define FATAL_H

#include <stdnoreturn.h>

noreturn void fatalxc(int excode, char const *fmt, ...);
#define fatal(...) fatalxc(EXIT_FAILURE, __VA_ARGS__)
noreturn void pfatalxc(int excode, char const *fmt, ...);
#define pfatal(...) pfatalxc(EXIT_FAILURE, __VA_ARGS__)

#endif
