#include "strto.h"
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdint.h>

int64_t strto_int64(const char *restrict nptr, char **restrict endptr, int base)
{
    errno = 0;
    intmax_t v = strtoimax(nptr, endptr, base);
    if (errno)
    {
        if (v == INTMAX_MAX) return INT64_MAX;
        if (v == INTMAX_MIN) return INT64_MIN;
        assert(v == 0);
        return 0;
    }
    if (v > INT64_MAX)
    {
        errno = ERANGE;
        return INT64_MAX;
    }
    if (v < INT64_MIN)
    {
        errno = ERANGE;
        return INT64_MIN;
    }
    return (int64_t)v;
}
