/* scrot_selection_classic.c

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
#include "selection_classic.h"

extern void selection_calculate_rect(int x0, int y0, int x1, int y1);
extern struct selection_t** selection_get(void);
extern void selection_set_line_width(struct selection_t* const sel);

struct selection_classic_t {
    XGCValues gcval;
    GC gc;
};

void selection_classic_create(void)
{
    struct selection_t *const sel = *selection_get();

    sel->classic = calloc(1, sizeof(struct selection_classic_t));

    struct selection_classic_t* pc = sel->classic;

    pc->gcval.function = GXxor;
    pc->gcval.foreground = XWhitePixel(disp, 0);
    pc->gcval.background = XBlackPixel(disp, 0);
    pc->gcval.plane_mask = pc->gcval.background ^ pc->gcval.foreground;
    pc->gcval.subwindow_mode = IncludeInferiors;

    if (opt.line_color != NULL) {
        XColor clr_exact, clr_closest;
        Status ret;

        ret = XAllocNamedColor(disp, XDefaultColormap(disp, DefaultScreen(disp)),
                opt.line_color, &clr_exact, &clr_closest);

        if (ret == 0) {
           free(opt.line_color);
           fprintf(stderr, "Error allocate color:%s\n", strerror(BadColor));
           scrot_selection_destroy();
           exit(EXIT_FAILURE);
        }

        pc->gcval.foreground = clr_exact.pixel;

        free(opt.line_color);
        opt.line_color = NULL;
    }

    pc->gc = XCreateGC(disp, root,
            GCFunction | GCForeground | GCBackground | GCSubwindowMode,
            &pc->gcval);

    assert(pc->gc != NULL);

    XSetLineAttributes(disp, pc->gc, opt.line_width, opt.line_style, CapRound, JoinRound);

    if (opt.freeze == 1) {
        XGrabServer(disp);
    }
}


void selection_classic_destroy(void)
{
    struct selection_t const *const sel = *selection_get();
    struct selection_classic_t* pc = sel->classic;
  
    if (opt.freeze == 1) {
        XUngrabServer(disp);
    }

    if (pc->gc) {
        XFreeGC(disp, pc->gc);
    }

    free(pc);
}


void selection_classic_draw(void)
{
    struct selection_t const *const sel = *selection_get();
    struct selection_classic_t const *const pc = sel->classic;
    XDrawRectangle(disp, root, pc->gc, sel->rect.x, sel->rect.y, sel->rect.w, sel->rect.h);
    XFlush(disp);
}


void selection_classic_motion_draw(int x0, int y0, int x1, int y1)
{
    struct selection_t *const sel = *selection_get();

    if (sel->rect.w) {
        selection_classic_draw();
    }
    selection_calculate_rect(x0, y0, x1, y1);
    selection_set_line_width(sel);
    selection_classic_draw();
}
