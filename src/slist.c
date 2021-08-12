/* slist.c

Copyright 2021      Christopher Nelson <christopher.nelson@languidnights.com>
Copyright 2021      Peter Wu <peterwu@hotmail.com>

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

#include "slist.h"
#include <Imlib2.h>
#include <stdlib.h>

ScrotImlibList* appendToScrotImlib(ScrotImlibList* head, Imlib_Image* data)
{
    ScrotImlibList* tail = walkToEndOfScrotImlibList(head);
    ScrotImlibList* appended;
    appended = malloc(sizeof(*appended));
    appended->data = data;
    appended->next = NULL;

    if (!head)
        return appended;

    tail->next = appended;
    return head;
}

ScrotImlibList* walkToEndOfScrotImlibList(ScrotImlibList* list)
{
    if (!list)
        return NULL;
    if (!list->next)
        return list;

    while (list->next)
        list = list->next;

    return list;
}

int isScrotImlibListEmpty(ScrotImlibList* list)
{
    if (!list)
        return 1;
    else
        return 0;
}
