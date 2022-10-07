#include "fatal.h"

#define _POSIX_C_SOURCE 200112L
#define _DARWIN_C_SOURCE 1
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>

noreturn void fatalxc(int excode, char const *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputs("\n", stderr);
    exit(excode);
}

noreturn void pfatalxc(int excode, char const *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    if (errno) fprintf(stderr, ": %s", strerror(errno));
    fputs("\n", stderr);
    exit(excode);
}
