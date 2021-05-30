/* scrot_selection.h

Copyright 2020  daltomi <daltomi@disroot.org>
Copyright 2020  Daniel T. Borelli <daltomi@disroot.org>
Copyright 2021  Cationiz3r

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

/*
    This file is part of the scrot project.
    Part of the code comes from the main.c file and maintains its authorship.
*/

#ifndef SCROT_SELECTION_H
#define SCROT_SELECTION_H

#include <assert.h>

#define LINE_MODE_CLASSIC       "classic"
#define LINE_MODE_CLASSIC_LEN   7
#define LINE_MODE_EDGE          "edge"
#define LINE_MODE_EDGE_LEN      4

struct selection_rect_t {
    int x, y, w, h;
};

struct selection_classic_t;
struct selection_edge_t;

struct selection_t {
    Cursor cur_cross, cur_angle_nw, cur_angle_ne, cur_angle_sw, cur_angle_se;

    struct selection_rect_t rect;
    struct selection_classic_t* classic;
    struct selection_edge_t* edge;

    void (*create)(void);
    void (*destroy)(void);
    void (*draw)(void);
    void (*motion_draw)(int x0, int y0, int x1, int y1);
};

void scrot_selection_create(void);
void scrot_selection_destroy(void);
void scrot_selection_draw(void);
void scrot_selection_motion_draw(int x0, int y0, int x1, int y1);
struct selection_rect_t* const scrot_selection_get_rect(void);

#endif
