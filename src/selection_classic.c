/* scrot_selection_classic.c

Copyright 2020-2021 Daniel T. Borelli <danieltborelli@gmail.com>
Copyright 2021-2023 Guilherme Janczak <guilherme.janczak@yandex.com>
Copyright 2021      Peter Wu <peterwu@hotmail.com>
Copyright 2023      NRK <nrk@disroot.org>

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
    Part of the code comes from the scrot.c file and maintains its authorship.
*/

#include <stdlib.h>
#include <err.h>

#include <X11/Xlib.h>

#include "options.h"
#include "scrot.h"
#include "scrot_selection.h"
#include "selection_classic.h"
#include "selection_edge.h"
#include "util.h"

void selectionClassicCreate(void)
{
    struct Selection *const sel = &selection;
    struct SelectionClassic *pc = &sel->classic;

    const unsigned long whiteColor = XWhitePixel(disp, 0);
    const unsigned long blackColor = XBlackPixel(disp, 0);

    pc->gcValues.function = GXxor;
    pc->gcValues.foreground = whiteColor;
    pc->gcValues.background = blackColor;
    pc->gcValues.plane_mask = pc->gcValues.background ^ pc->gcValues.foreground;
    pc->gcValues.subwindow_mode = IncludeInferiors;

    XColor color;
    scrotSelectionGetLineColor(&color);

    if (color.pixel != blackColor)
        pc->gcValues.foreground = color.pixel;

    pc->gc = XCreateGC(disp, root,
        GCFunction | GCForeground | GCBackground | GCSubwindowMode,
        &pc->gcValues);
    if (pc->gc == NULL)
        errx(EXIT_FAILURE, "failed to create GC");

    XSetLineAttributes(disp, pc->gc, opt.lineWidth, opt.lineStyle, CapRound,
        JoinRound);

    if (opt.freeze)
        XGrabServer(disp);
}

void selectionClassicDraw(void)
{
    const struct Selection *const sel = &selection;
    const struct SelectionClassic *const pc = &sel->classic;
    XDrawRectangle(disp, root, pc->gc, sel->rect.x, sel->rect.y, sel->rect.w,
        sel->rect.h);
    XFlush(disp);
}

void selectionClassicMotionDraw(int x0, int y0, int x1, int y1)
{
    const struct Selection *const sel = &selection;

    if (sel->rect.w)
        selectionClassicDraw();
    selectionCalculateRect(x0, y0, x1, y1);
    selectionClassicDraw();
}

void selectionClassicDestroy(void)
{
    const struct Selection *const sel = &selection;
    const struct SelectionClassic *pc = &sel->classic;

    if (opt.freeze)
        XUngrabServer(disp);

    if (pc->gc)
        XFreeGC(disp, pc->gc);

    XFlush(disp);
}
