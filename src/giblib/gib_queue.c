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

#include "gib_queue.h"
#include "gib_utils.h"
#include "gib_debug.h"

gib_queue *gib_queue_new()
{
	gib_queue *q = gib_emalloc(sizeof(gib_queue));
	q->base = NULL;
	return q;
}

void       gib_queue_free(gib_queue *queue)
{
	gib_list_free(queue->base);
	gib_efree(queue);
	return;
}

void       gib_queue_add(gib_queue *queue, void *data)
{
	queue->base = gib_list_add_end(queue->base, data);
	return;
}

void          *gib_queue_next(gib_queue *queue)
{
	gib_list *head = queue->base;
	void     *data;
	
	if (head) {
		data = head->data;
		queue->base = head->next;
		gib_efree(head);
		return data;
	}
	return NULL;
}

void          *gib_queue_peek(gib_queue *queue)
{
	return gib_queue_pending(queue)?queue->base->data:NULL;
}

unsigned char  gib_queue_pending(gib_queue *queue)
{
	return queue->base?1:0;
}
