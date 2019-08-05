/* gib_btree.c

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
#include "gib_btree.h"
#include "gib_utils.h"
#include "gib_debug.h"

gib_btree *gib_btree_new(void *data, int sort_val)
{
	gib_btree *t = gib_emalloc(sizeof(gib_btree));
	t->data = data;
	t->val = sort_val;
	t->left = NULL;
	t->right = NULL;
	return t;
}

void       gib_btree_free(gib_btree *tree)
{
	if (tree->left) gib_btree_free(tree->left);
	if (tree->right) gib_btree_free(tree->right);
	gib_efree(tree);
	return;
}

void       gib_btree_free_and_data(gib_btree *tree)
{
	if (tree->left) gib_btree_free_and_data(tree->left);
	if (tree->right) gib_btree_free_and_data(tree->right);
	gib_efree(tree->data);
	gib_efree(tree);
	return;
}

void       gib_btree_free_leaf(gib_btree *leaf)
{
	gib_efree(leaf);
	return;
}

void       gib_btree_free_leaf_and_data(gib_btree *leaf)
{
	gib_efree(leaf->data);
	gib_efree(leaf);
	return;
}

gib_btree *gib_btree_add(gib_btree *tree, void *data, int sort_val)
{
	return gib_btree_add_branch(tree, gib_btree_new(data,sort_val));
}

gib_btree *gib_btree_add_branch(gib_btree *tree, gib_btree *branch)
{
	gib_btree *i, *left, *right, *next;

	if (!tree) return branch;

	left = branch->left;
	right = branch->right;
	branch->left = NULL;
	branch->right = NULL;

	for (i=tree; i; i=next) {
		if (branch->val<i->val) {
			if (i->left) {
				next = i->left;
			} else {
				i->left = branch;
				next = NULL;
			}
		} else {
			if (i->right) {
				next = i->right;
			} else {
				i->right = branch;
				next = NULL;
			}
		}
	}

	if (left) gib_btree_add_branch(tree, left);
	if (right) gib_btree_add_branch(tree, right);

	return tree;
}

gib_btree *gib_btree_remove(gib_btree *tree, gib_btree *leaf)
{
	gib_btree *i, *temp;

	if (tree==leaf) {
		gib_btree_free(leaf);
		return NULL;
	}
	
	for (i=tree; i; i=(leaf->val<i->val)?i->left:i->right)	{
		if (i->left==leaf) {
			if (leaf->right) {
				temp = leaf->left;
				i->left = leaf->right;
				tree = gib_btree_add_branch(tree, temp);
			}
			gib_btree_free_leaf(leaf);
			break;
		} else if (i->right==leaf) {
			if (leaf->right) {
				temp = leaf->left;
				i->right = leaf->right;
				tree = gib_btree_add_branch(tree, temp);
			}
			gib_btree_free_leaf(leaf);
			break;
		}
	}
	return tree;
}

gib_btree *gib_btree_remove_branch(gib_btree *tree, gib_btree *branch)
{
	gib_btree *i;

	if (tree==branch) {
		gib_btree_free(branch);
		return NULL;
	}
	
	for (i=tree; i; i=(branch->val<i->val)?i->left:i->right)	{
		if (i->left==branch) {
			gib_btree_free(branch);
			i->left = NULL;
			break;
		} else if (i->right==branch) {
			gib_btree_free(branch);
			i->right = NULL;
			break;
		}
	}
	return tree;
}

gib_btree *gib_btree_find(gib_btree *tree, int val)
{
	gib_btree *i;
	
	for (i=tree; i; i= (val<i->val)?i->left:i->right)	{
		if (i->val==val)
			return i;
	}

	return NULL;
}

gib_btree *gib_btree_find_by_data(gib_btree *tree, unsigned char (*find_func)(gib_btree *tree, void *data), void *data)
{
	gib_btree *r = NULL;

	if (!tree) return NULL;	

	if (find_func(tree,data))
		return tree;
	r = gib_btree_find_by_data(tree->left,find_func,data);
	if (r)
		return r;
	r = gib_btree_find_by_data(tree->right,find_func,data);
	if (r)
		return r;
	
	return NULL;

}

void      gib_btree_traverse(gib_btree *tree, void (*traverse_cb)(gib_btree *tree, void *data), int order, void *data)
{
	if (!tree)
		return;
	
	switch (order) {
	case GIB_PRE:
		traverse_cb(tree, data);
		gib_btree_traverse(tree->left, traverse_cb, order, data);
		gib_btree_traverse(tree->right, traverse_cb, order, data);
		break;
	case GIB_IN:
		gib_btree_traverse(tree->left, traverse_cb, order, data);
		traverse_cb(tree, data);
		gib_btree_traverse(tree->right, traverse_cb, order, data);
		break;
	case GIB_POST:
		gib_btree_traverse(tree->left, traverse_cb, order, data);
		gib_btree_traverse(tree->right, traverse_cb, order, data);
		traverse_cb(tree, data);
		break;
	default:
		/* um... well shit */
		fprintf(stderr,"giblib_btree: unknown traverse order %d.\n", order);
	}

	return;
}
