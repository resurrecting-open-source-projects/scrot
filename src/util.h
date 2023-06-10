/* util.h

Copyright 2021-2023 Guilherme Janczak <guilherme.janczak@yandex.com>
Copyright 2023      NRK <nrk@disroot.org>

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

#ifndef H_UTIL
#define H_UTIL

#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG
    #define scrotAssert(X) do { \
        if (!(X)) { \
            fprintf(stderr, "%s:%d: `%s`\n", __FILE__, __LINE__, #X); \
            abort(); \
        } \
    } while (0)
#else
    #define scrotAssert(X) ((void)0)
#endif

#define ARRAY_COUNT(X)   (sizeof(X) / sizeof(0[X]))
#define MAX(A, B)        ((A) > (B) ? (A) : (B))

typedef struct {
    char *buf;
    size_t off, cap;
} Stream;

char *estrdup(const char *);
void *ecalloc(size_t, size_t);
void *erealloc(void *, size_t);

void streamReserve(Stream *, size_t);
void streamChar(Stream *, char);
void streamMem(Stream *, const void *, size_t);
void streamStr(Stream *, const char *);

#endif /* !defined(H_UTIL) */
