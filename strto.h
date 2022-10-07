#ifndef STRTO_H
#define STRTO_H

#include <stdint.h>

int64_t strto_int64(const char *restrict nptr, char **restrict endptr,
                    int base);

#endif
