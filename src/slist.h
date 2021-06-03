/* structs.h

Copyright 2021      Christopher Nelson <christopher.nelson@languidnights.com>

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

#ifndef SCROT_SLIST_H
#define SCROT_SLIST_H

#include <Imlib2.h>
#include <stdbool.h>

typedef struct Scrot_Imlib_List {
	Imlib_Image * data;
	
	struct Scrot_Imlib_List * next;
} Scrot_Imlib_List;

Scrot_Imlib_List * append_to_scrot_imlib(Scrot_Imlib_List *head, Imlib_Image *data);
Scrot_Imlib_List * walk_to_end_of_scrot_imlib_list(Scrot_Imlib_List *list);
int is_scrot_imlib_list_empty(Scrot_Imlib_List *list);

#endif
