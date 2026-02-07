/* scrot_selection_edge.c

Copyright 2020-2021 Daniel T. Borelli <danieltborelli@gmail.com>
Copyright 2021-2023 Guilherme Janczak <guilherme.janczak@yandex.com>
Copyright 2021      Peter Wu <peterwu@hotmail.com>
Copyright 2023-2026 NRK <nrk@disroot.org>

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

#include <errno.h>
#include <stdlib.h>
#include <time.h>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "options.h"
#include "scrot.h"
#include "scrot_selection.h"
#include "selection_edge.h"
#include "util.h"

void selectionEdgeCreate(void)
{
    struct SelectionEdge *const pe = &selection.edge;

    XColor color = scrotSelectionGetLineColor();

    XSetWindowAttributes attr;
    attr.background_pixel = color.pixel;
    attr.override_redirect = True;
    Atom winType = XInternAtom(disp, "_NET_WM_WINDOW_TYPE", False);
    Atom winDock = XInternAtom(disp, "_NET_WM_WINDOW_TYPE_DOCK", False);
    Atom winOpacity = XInternAtom(disp, "_NET_WM_WINDOW_OPACITY", False);

    for (size_t i = 0; i < ARRAY_COUNT(pe->windows); ++i) {
        pe->windows[i] = XCreateWindow(disp, root,
            0, 0, WidthOfScreen(scr), HeightOfScreen(scr), 0,
            CopyFromParent, InputOutput, CopyFromParent,
            CWOverrideRedirect | CWBackPixel, &attr);

        XChangeProperty(disp, pe->windows[i], winType, XA_ATOM, 32,
            PropModeReplace, (unsigned char *)&winDock, 1L);

        unsigned long opacity = opt.lineOpacity * (0xFFFFFFFFu / 255);
        XChangeProperty(disp, pe->windows[i], winOpacity, XA_CARDINAL, 32,
            PropModeReplace, (unsigned char *)&opacity, 1L);

        XClassHint hint = { .res_name = "scrot", .res_class = "scrot" };
        XSetClassHint(disp, pe->windows[i], &hint);
    }
    pe->isMapped = false;

}

void selectionEdgeDraw(void)
{
    struct Selection *const sel = &selection;
    struct SelectionEdge *const pe = &sel->edge;

    XRectangle rects[4] = {
        { sel->rect.x, sel->rect.y + opt.lineWidth,
            opt.lineWidth, sel->rect.h - opt.lineWidth }, // left
        { sel->rect.x, sel->rect.y, sel->rect.w, opt.lineWidth }, // top
        { sel->rect.x + sel->rect.w, sel->rect.y, opt.lineWidth, sel->rect.h }, // right
        { sel->rect.x, sel->rect.y + sel->rect.h, sel->rect.w + opt.lineWidth,
            opt.lineWidth } // bottom
    };

    if (sel->rect.w == 0 || sel->rect.h == 0)
        return;

    for (size_t i = 0; i < ARRAY_COUNT(pe->windows); ++i) {
        XRectangle *rp = rects + i;
        XMoveResizeWindow(disp, pe->windows[i], rp->x, rp->y, rp->width, rp->height);
        XMapWindow(disp, pe->windows[i]);
    }
    pe->isMapped = true;
}

void selectionEdgeMotionDraw(int x0, int y0, int x1, int y1)
{
    struct Selection *const sel = &selection;

    selectionCalculateRect(x0, y0, x1, y1);

    sel->rect.x -= opt.lineWidth;
    sel->rect.y -= opt.lineWidth;
    sel->rect.w += opt.lineWidth;
    sel->rect.h += opt.lineWidth;

    selectionEdgeDraw();
}

void selectionEdgeDestroy(void)
{
    const struct SelectionEdge *pe = &selection.edge;

    for (size_t i = 0; i < ARRAY_COUNT(pe->windows); ++i) {
        if (pe->windows[i] == None)
            continue;
        XSelectInput(disp, pe->windows[i], StructureNotifyMask);
        XDestroyWindow(disp, pe->windows[i]);
        bool isUnmapped = !pe->isMapped, isDestroyed = false;
        for (XEvent ev; !(isUnmapped && isDestroyed);) {
            XNextEvent(disp, &ev);
            if (ev.type == DestroyNotify && ev.xdestroywindow.window == pe->windows[i])
                isDestroyed = true;
            if (ev.type == UnmapNotify && ev.xunmap.window == pe->windows[i])
                isUnmapped = true;
        }
    }
}
