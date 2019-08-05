/* gib_btree.h

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

#ifndef _GIB_BTREE_H
#define _GIB_BTREE_H

#include "config.h"

#define GIB_BTREE(a) ((gib_btree*)a)

typedef struct __gib_btree gib_btree;

struct __gib_btree {
	void  *data;
	int    val;
	gib_btree *left,
	          *right;
};

enum _gib_btree_traverse_order {
	GIB_PRE,
	GIB_IN,
	GIB_POST
};

#ifdef __cplusplus
extern "C"
{
#endif

gib_btree *gib_btree_new(void *data, int sort_val);
void       gib_btree_free(gib_btree *tree);
void       gib_btree_free_and_data(gib_btree *tree);

void       gib_btree_free_leaf(gib_btree *tree);
void       gib_btree_free_leaf_and_data(gib_btree *tree);

gib_btree *gib_btree_add(gib_btree *tree, void *data, int sort_val);
gib_btree *gib_btree_add_branch(gib_btree *tree, gib_btree *branch);
gib_btree *gib_btree_remove(gib_btree *tree, gib_btree *leaf);
gib_btree *gib_btree_remove_branch(gib_btree *tree, gib_btree *branch);

gib_btree *gib_btree_find(gib_btree *tree, int val);
gib_btree *gib_btree_find_by_data(gib_btree *tree, unsigned char (*find_func)(gib_btree *tree, void *data), void *data);

void      gib_btree_traverse(gib_btree *tree, void (*traverse_cb)(gib_btree *tree, void *data), int order, void *data);

#ifdef __cplusplus
}
#endif

#endif /* _BIG_BTREE_H */
