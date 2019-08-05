/* debug.h

Copyright (C) 1999,2000 Tom Gilbert.

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

#ifndef DDEBUG_H
#define DDEBUG_H

#include "config.h"

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#define gib_emalloc(a) malloc(a)
#define gib_efree(a) free(a)
#define gib_estrdup(a) strdup(a)
#define gib_erealloc(a,b) realloc(a,b)
#else
#define gib_emalloc(a) _gib_emalloc(a)
#define gib_efree(a) _gib_efree(a)
#define gib_estrdup(a) _gib_estrdup(a)
#define gib_erealloc(a,b) _gib_erealloc(a,b)
#endif

#endif
