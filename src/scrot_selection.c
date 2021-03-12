/* scrot_selection.c

Copyright 2020  daltomi <daltomi@disroot.org>
Copyright 2020  Daniel T. Borelli <daltomi@disroot.org>

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

#include "scrot.h"
#include "selection_classic.h"
#include "selection_edge.h"

struct selection_t** selection_get(void)
{
    static struct selection_t* sel = NULL;
    return &sel;
}

static void selection_allocate(void)
{
    struct selection_t** sel = selection_get();
    *sel = calloc(1, sizeof(struct selection_t));
}

static void selection_deallocate(void)
{
    struct selection_t** sel = selection_get();
    free(*sel);
    *sel = NULL;
}


void selection_calculate_rect(int x0, int y0, int x1, int y1)
{
    struct selection_rect_t* const rect = scrot_selection_get_rect();

    rect->x = x0;
    rect->y = y0;
    rect->w = x1 - x0;
    rect->h = y1 - y0;

    if (rect->w == 0) rect->w++;

    if (rect->h == 0) rect->h++;

    if (rect->w < 0) {
      rect->x += rect->w;
      rect->w = 0 - rect->w;
    }

    if (rect->h < 0) {
      rect->y += rect->h;
      rect->h = 0 - rect->h;
    }
}

void scrot_selection_create(void)
{
    selection_allocate();

    struct selection_t *const sel = *selection_get();

    assert(sel != NULL);

    sel->cur_cross = XCreateFontCursor(disp, XC_cross);
    sel->cur_angle = XCreateFontCursor(disp, XC_lr_angle);

    if (0 == strncmp(opt.line_mode, LINE_MODE_CLASSIC, LINE_MODE_CLASSIC_LEN)) {
        sel->create         = selection_classic_create;
        sel->draw           = selection_classic_draw;
        sel->motion_draw    = selection_classic_motion_draw;
        sel->destroy        = selection_classic_destroy;
    } else if (0 == strncmp(opt.line_mode, LINE_MODE_EDGE, LINE_MODE_EDGE_LEN)) {
        sel->create         = selection_edge_create;
        sel->draw           = selection_edge_draw;
        sel->motion_draw    = selection_edge_motion_draw;
        sel->destroy        = selection_edge_destroy;
    } else {
        // It never happened, fix the options.c file
        assert(0);
    }

    sel->create();

    unsigned int const EVENT_MASK = ButtonMotionMask | ButtonPressMask | ButtonReleaseMask;

    if ((XGrabPointer (disp, root, False, EVENT_MASK, GrabModeAsync,
        GrabModeAsync, root, sel->cur_cross, CurrentTime) != GrabSuccess)) {
        fprintf(stderr, "couldn't grab pointer\n");
        scrot_selection_destroy();
        exit(EXIT_FAILURE);
    }
}


void scrot_selection_destroy(void)
{
    struct selection_t *const sel = *selection_get();
    XUngrabPointer(disp, CurrentTime);
    XFreeCursor(disp, sel->cur_cross);
    XFreeCursor(disp, sel->cur_angle);
    XSync(disp, True);
    sel->destroy();
    selection_deallocate();
}


void scrot_selection_draw(void)
{
    struct selection_t const *const sel = *selection_get();
    sel->draw();
}


void scrot_selection_motion_draw(int x0, int y0, int x1, int y1)
{
    struct selection_t const *const sel = *selection_get();
    unsigned int const EVENT_MASK = ButtonMotionMask | ButtonReleaseMask;
    XChangeActivePointerGrab(disp, EVENT_MASK, sel->cur_angle, CurrentTime);
    sel->motion_draw(x0, y0, x1, y1);
}

struct selection_rect_t* const scrot_selection_get_rect(void)
{
    return &(*selection_get())->rect;
}
