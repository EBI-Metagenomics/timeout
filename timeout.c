#include "argless.h"
#include "fatal.h"
#include "mssleep.h"
#include "pp.h"
#include "strto.h"
#include "warn.h"

// Acknowledgement: busybox, coreutils, toybox

#define _POSIX_C_SOURCE 200112L
#define _DARWIN_C_SOURCE 1
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TIMEOUT_DEFAULT 100
#define KTIMEOUT_DEFAULT 0

static struct argl_option const options[] = {
    {"timeout", 't', ARGL_TEXT("MILLISECONDS", STRINGIFY(TIMEOUT_DEFAULT)),
     "Milliseconds to timeout."},
    {"ktimeout", 'k', ARGL_TEXT("MILLISECONDS", STRINGIFY(KTIMEOUT_DEFAULT)),
     "Milliseconds to force-kill."},
    ARGL_DEFAULT,
    ARGL_END,
};

static struct argl argl = {.options = options,
                           .args_doc = "<program> [arguments ...]",
                           .doc = "Timeout a program.",
                           .version = "0.0.1"};

static pid_t grandparent_pid = 0;
static int64_t timeout = TIMEOUT_DEFAULT;
static int64_t ktimeout = KTIMEOUT_DEFAULT;

static char **parse_main_args(int argc, char *argv[]);
static void noreturn grandparent(char const *program, char **args);
static void noreturn parent(void);
static void noreturn grandchild(void);

int main(int argc, char *argv[])
{
    char **args = parse_main_args(argc, argv);
    char const *program = args[0];

    pid_t pid = fork();
    if (pid < 0) fatal("fork failed");
    if (pid > 0) grandparent(program, args);
    parent();

    return 0;
}

static char **parse_main_args(int argc, char *argv[])
{
    argl_parse(&argl, argc, argv);
    if (argl_nargs(&argl) == 0) argl_usage(&argl);
    timeout = strto_int64(argl_get(&argl, "timeout"), NULL, 10);
    if (errno || timeout < 0) fatal("failed to parse milliseconds");
    ktimeout = strto_int64(argl_get(&argl, "ktimeout"), NULL, 10);
    if (errno || timeout < 0) fatal("failed to parse milliseconds");
    return argl_args(&argl);
}

static void noreturn grandparent(char const *program, char **args)
{
    pid_t status = 0;
    wait(&status);
    if (!WIFEXITED(status) || WEXITSTATUS(status)) exit(EXIT_FAILURE);
    execvp(program, args);
    pfatal("failed to execute %s", program);
}

static void noreturn parent(void)
{
    grandparent_pid = getppid();
    pid_t pid = fork();
    if (pid < 0) fatal("fork failed");
    if (pid > 0) exit(EXIT_SUCCESS);
    grandchild();
}

static int timeout_wait(int64_t timeout, pid_t pid);

static void noreturn grandchild(void)
{
    if (!timeout_wait(timeout, grandparent_pid)) exit(EXIT_SUCCESS);
    kill(grandparent_pid, SIGTERM);
    if (!timeout_wait(100, grandparent_pid)) exit(EXIT_SUCCESS);
    kill(grandparent_pid, SIGKILL);
    exit(EXIT_SUCCESS);
}

static int timeout_wait(int64_t timeout, pid_t pid)
{
    int64_t step = timeout / 100;
    step = step < 10 ? 10 : step;
    step = step > 1000 ? 1000 : step;
    while (1)
    {
        mssleep(step);
        timeout -= step;
        if (timeout <= 0) break;
        if (kill(pid, 0)) return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}
