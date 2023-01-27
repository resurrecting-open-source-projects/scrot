/*

Copyright 2023 Guilherme Janczak <guilherme.janczak@yandex.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include <sys/time.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include "options.h"

/* Initialized to invalid fds. */
static int selfPipe[] = {-1, -1};

static void sigalrm(int, siginfo_t *, void *);
static void timerEnd(void);

/* scrotTimerStart: start counting seconds. */
void
scrotTimerStart(void)
{
    struct sigaction act;
    struct itimerval value = {0};

    /* Ignore repeated attempts to set up. */
    if (selfPipe[0] != -1)
        return;

    /* This is the self pipe trick: https://cr.yp.to/docs/selfpipe.html
     * In C99, due to the limitations of signal handlers, this is the only way
     * to communicate how many times the signal has been triggered.
     * In C11, we could use <stdatomic.h>.
     *
     * OS X doesn't have pipe2(), so we use pipe() and fcntl().
     */
    if (pipe(selfPipe) == -1)
        err(1, "pipe");
    fcntl(selfPipe[0], O_NONBLOCK);

    act.sa_sigaction = sigalrm;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGALRM, &act, NULL);

    /* We trigger the signal every second regardless of whether we're counting
     * down or not. This simplifies the code.
     */
    value.it_value.tv_sec = value.it_interval.tv_sec = 1;
    setitimer(ITIMER_REAL, &value, NULL);

    return;
}

static void
sigalrm(int sig, siginfo_t *info, void *unused)
{
    int savedErrno;

#if defined(__OpenBSD__)
    /* OpenBSD's singinfo_t functionality is unimplemented, the structure it
     * passes is us set to meaningless zeroes. Open an issue or PR if your OS
     * has the same problem.
     */
    info->si_code = SI_TIMER;
#endif
    if (info->si_code != SI_TIMER)
        return; /* Someone gave us a SIGALRM. Ignore it. */

    (void)unused;

    savedErrno = errno;
    while (write(selfPipe[1], &sig /* Irrelevant value. */, 1) == -1) {
        if (errno == EINTR)
            continue;

        /* Hangup the pipe to signal error. The only C99 ways to communicate the
         * exact errno value would be to pass it through a pipe or write a
         * kludge that writes it to multiple sig_atomic_t variables and we just
         * failed to write to a pipe.
         *
         * In practice, this will never happen.
         */
        close(selfPipe[1]);
        break;
    }
    errno = savedErrno;
}

/* scrotTimerWait: Wait for the delay, possibly printing a countdown.
 *
 * The contdown is only printed if opt.countdown is set.
 */
void
scrotTimerWait(void)
{
    struct pollfd pfd;
    unsigned char unusedBuf;

    if (opt.countdown) {
        printf("Taking screenshot in %d.. ", opt.delay);
        fflush(stdout);
    }

    pfd.fd = selfPipe[0];
    pfd.events = POLLIN;
    for (; opt.delay > 0;) {
        if (poll(&pfd, 1, -1) == -1) {
            if (errno == EINTR)
                continue;
            err(1, "poll");
        }
        opt.delay--;

        /*
         * POSIX allows POLLIN and POLLHUP to be set simultaneously, but dealing
         * with that theoretical edge case is not worth it, so just error if we
         * see POLLHUP.
         */
        if (pfd.revents & POLLHUP)
            errx(1, "sigalrm() failed to write to selfPipe");

        if (pfd.revents & POLLIN) {
            while (read(selfPipe[0], &unusedBuf, 1) == -1) {
                if (errno == EINTR)
                    continue;
                err(1, "read");
            }
            if (opt.countdown) {
                printf("%d.%s ", opt.delay, opt.delay ? "." : "");
                fflush(stdout);
            }
        }
    }
    if (opt.countdown)
        putchar('\n');

    timerEnd();
}

static void
timerEnd(void)
{
    struct sigaction act;
    struct itimerval value = {0};
    setitimer(ITIMER_REAL, &value, NULL);

    act.sa_handler = SIG_DFL;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGALRM, &act, NULL);

    close(selfPipe[0]);
    close(selfPipe[1]);
    selfPipe[0] = selfPipe[1] = -1;
}
