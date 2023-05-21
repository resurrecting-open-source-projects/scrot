/* util.c

Copyright 2021 Guilherme Janczak <guilherme.janczak@yandex.com>
Copyright 2023 NRK <nrk@disroot.org>

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

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

char *estrdup(const char *str)
{
    char *p = strdup(str);
    if (!p)
        err(EXIT_FAILURE, "strdup");
    return p;
}

void *ecalloc(size_t nmemb, size_t size)
{
    void *p = calloc(nmemb, size);
    if (!p)
        err(EXIT_FAILURE, "calloc");
    return p;
}

void *erealloc(void *ptr, size_t size)
{
    void *p = realloc(ptr, size);
    if (!p)
        err(EXIT_FAILURE, "realloc");
    return p;
}

void streamReserve(Stream *buf, size_t size)
{
    scrotAssert(buf->off <= buf->cap);
    size_t avail = buf->cap - buf->off;
    if (avail < size) {
        buf->cap += MAX(size, 128); /* be a bit greedy when allocating */
        buf->buf = erealloc(buf->buf, buf->cap);
    }
    scrotAssert((buf->off + size) <= buf->cap);
}

void streamChar(Stream *buf, char ch)
{
    streamReserve(buf, 1);
    buf->buf[buf->off++] = ch;
}

void streamMem(Stream *buf, const void *mem, size_t n)
{
    streamReserve(buf, n);
    memcpy(buf->buf + buf->off, mem, n);
    buf->off += n;
}

void streamStr(Stream *buf, const char *str)
{
    streamMem(buf, str, strlen(str));
}
