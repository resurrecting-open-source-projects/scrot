/* gib_stack.h

Copyright (C) 1999,2000 Paul Duncan.

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


#ifndef _GIB_STACK_H_
#define _GIB_STACK_H_

#include "config.h"
#include "gib_list.h"

#define GIB_STACK(a) ((gib_stack*)a)

typedef struct __gib_stack gib_stack;

struct __gib_stack {
	gib_list *base;
};

#ifdef __cplusplus
extern "C"
{
#endif

gib_stack *gib_stack_new();
void       gib_stack_free(gib_stack *stack);

void       gib_stack_push(gib_stack *stack, void *data);
void      *gib_stack_pop(gib_stack *stack);
void      *gib_stack_peek(gib_stack *stack);

unsigned char  gib_stack_pending(gib_stack *stack);

#ifdef __cplusplus
}
#endif


#endif /*_GIB_QUEUE_H_*/
