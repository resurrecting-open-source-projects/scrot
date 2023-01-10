/* slist.h

Copyright 2021 Christopher Nelson <christopher.nelson@languidnights.com>
Copyright 2021 Daniel T. Borelli <danieltborelli@gmail.com>
Copyright 2021 Peter Wu <peterwu@hotmail.com>

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

#pragma once

#include <sys/queue.h>

typedef struct ScrotListNode {
    void *data;
    STAILQ_ENTRY(ScrotListNode) nodes;
} ScrotListNode;

typedef STAILQ_HEAD(ScrotLists, ScrotListNode) ScrotList;

#define initializeScrotList(name)                   \
    ScrotList name = STAILQ_HEAD_INITIALIZER(name); \
    STAILQ_INIT(&name);

#define appendToScrotList(name, newData) do {       \
    ScrotListNode *node = calloc(1, sizeof(*node)); \
    node->data = (void *)newData;                    \
    STAILQ_INSERT_TAIL(&name, node, nodes);         \
} while(0)

#define isEmptyScrotList(name) \
    STAILQ_EMPTY(name)

#define forEachScrotList(name, node) \
    STAILQ_FOREACH(node, name, nodes)

#define firstScrotList(name) \
    STAILQ_FIRST(name)

#define nextScrotList(name) \
    STAILQ_NEXT(name, nodes);

#define nextAndFreeScrotList(name) do {         \
    ScrotListNode *next = nextScrotList(name);  \
    free(name);                                 \
    name = next;                                \
} while(0)

