/* scrot_selection_classic.c

Copyright 2020-2021 Daniel T. Borelli <daltomi@disroot.org>
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

/*
    This file is part of the scrot project.
    Part of the code comes from the main.c file and maintains its authorship.
*/
#include "selection_classic.h"

extern void selectionCalculateRect(int, int, int, int);
extern struct Selection** selectionGet(void);

struct SelectionClassic {
    XGCValues gcValues;
    GC gc;
};

void selectionClassicCreate(void)
{
    struct Selection* const sel = *selectionGet();

    sel->classic = calloc(1, sizeof(*sel->classic));

    struct SelectionClassic* pc = sel->classic;

    pc->gcValues.function = GXxor;
    pc->gcValues.foreground = XWhitePixel(disp, 0);
    pc->gcValues.background = XBlackPixel(disp, 0);
    pc->gcValues.plane_mask = pc->gcValues.background ^ pc->gcValues.foreground;
    pc->gcValues.subwindow_mode = IncludeInferiors;

    if (opt.lineColor) {
        XColor clrExact, clrClosest;
        Status ret;

        ret = XAllocNamedColor(disp, XDefaultColormap(disp, DefaultScreen(disp)),
            opt.lineColor, &clrExact, &clrClosest);

        if (!ret) {
            free(opt.lineColor);
            fprintf(stderr, "Error allocate color:%s\n", strerror(BadColor));
            scrotSelectionDestroy();
            exit(EXIT_FAILURE);
        }

        pc->gcValues.foreground = clrExact.pixel;

        free(opt.lineColor);
        opt.lineColor = NULL;
    }

    pc->gc = XCreateGC(disp, root,
        GCFunction | GCForeground | GCBackground | GCSubwindowMode,
        &pc->gcValues);

    assert(pc->gc != NULL);

    XSetLineAttributes(disp, pc->gc, opt.lineWidth, opt.lineStyle, CapRound, JoinRound);

    if (opt.freeze)
        XGrabServer(disp);
}

void selectionClassicDestroy(void)
{
    struct Selection const* const sel = *selectionGet();
    struct SelectionClassic* pc = sel->classic;

    if (opt.freeze)
        XUngrabServer(disp);

    if (pc->gc)
        XFreeGC(disp, pc->gc);

    free(pc);
}

void selectionClassicDraw(void)
{
    struct Selection const* const sel = *selectionGet();
    struct SelectionClassic const* const pc = sel->classic;
    XDrawRectangle(disp, root, pc->gc, sel->rect.x, sel->rect.y, sel->rect.w, sel->rect.h);
    XFlush(disp);
}

void selectionClassicMotionDraw(int x0, int y0, int x1, int y1)
{
    struct Selection const* const sel = *selectionGet();

    if (sel->rect.w)
        selectionClassicDraw();
    selectionCalculateRect(x0, y0, x1, y1);
    selectionClassicDraw();
}
