#define _POSIX_C_SOURCE 200809L
#define _DARWIN_C_SOURCE 1
#include "mssleep.h"
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>

/* Source: https://tinycthread.github.io */
#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
#define SLEEP_WINDOWS 1
#else
#define SLEEP_POSIX 1
#endif

#ifdef SLEEP_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif

#ifndef TIME_UTC
#error Undefined TIME_UTC
#endif

struct timespec;

/* source: libbsd */
#ifndef timespecsub
#define timespecsub(tsp, usp, vsp)                                             \
    do                                                                         \
    {                                                                          \
        (vsp)->tv_sec = (tsp)->tv_sec - (usp)->tv_sec;                         \
        (vsp)->tv_nsec = (tsp)->tv_nsec - (usp)->tv_nsec;                      \
        if ((vsp)->tv_nsec < 0)                                                \
        {                                                                      \
            (vsp)->tv_sec--;                                                   \
            (vsp)->tv_nsec += 1000000000L;                                     \
        }                                                                      \
    } while (0)
#endif

#define SEC_IN_SEC 1
#define SEC_IN_MILLISEC (SEC_IN_SEC * 1000)
#define SEC_IN_MICROSEC (SEC_IN_MILLISEC * 1000)
#define SEC_IN_NANOSEC (SEC_IN_MICROSEC * 1000)

/* Source: https://tinycthread.github.io */
static int specsleep(const struct timespec *duration,
                     struct timespec *remaining)
{
#if defined(SLEEP_POSIX)
    int res = nanosleep(duration, remaining);
    if (res == 0)
    {
        return 0;
    }
    else if (errno == EINTR)
    {
        return -1;
    }
    else
    {
        return -2;
    }
#elif defined(SLEEP_WINDOWS)
    struct timespec start;
    DWORD t;

    timespec_get(&start, TIME_UTC);

    t = SleepEx((DWORD)(duration->tv_sec * 1000 + duration->tv_nsec / 1000000 +
                        (((duration->tv_nsec % 1000000) == 0) ? 0 : 1)),
                TRUE);

    if (t == 0)
    {
        return 0;
    }
    else
    {
        if (remaining != NULL)
        {
            timespec_get(remaining, TIME_UTC);
            remaining->tv_sec -= start.tv_sec;
            remaining->tv_nsec -= start.tv_nsec;
            if (remaining->tv_nsec < 0)
            {
                remaining->tv_nsec += 1000000000;
                remaining->tv_sec -= 1;
            }
        }

        return (t == WAIT_IO_COMPLETION) ? -1 : -2;
    }
#else
#error Unknown standard
#endif
}

int mssleep(int64_t ms)
{
    struct timespec t = {.tv_sec = (time_t)(ms / SEC_IN_MILLISEC),
                         .tv_nsec = (long)((ms % SEC_IN_MILLISEC) * 1000000)};
    return specsleep(&t, NULL);
}
