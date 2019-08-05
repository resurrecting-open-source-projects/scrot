/* gib_stack.c

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

#include "gib_stack.h"
#include "gib_utils.h"
#include "gib_debug.h"

gib_stack *gib_stack_new()
{
	gib_stack *q = gib_emalloc(sizeof(gib_stack));
	q->base = NULL;
	return q;
}

void       gib_stack_free(gib_stack *stack)
{
	gib_list_free(stack->base);
	gib_efree(stack);
	return;
}

void       gib_stack_push(gib_stack *stack, void *data)
{
	stack->base = gib_list_add_front(stack->base, data);
	return;
}

void          *gib_stack_pop(gib_stack *stack)
{
	gib_list *head = stack->base;
	void     *data;
	
	if (head) {
		data = head->data;
		stack->base = head->next;
		gib_efree(head);
		return data;
	}
	return NULL;
}

void          *gib_stack_peek(gib_stack *stack)
{
	return gib_stack_pending(stack)?stack->base->data:NULL;
}

unsigned char  gib_stack_pending(gib_stack *stack)
{
	return stack->base?1:0;
}
