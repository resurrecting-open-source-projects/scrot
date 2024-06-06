/* scrot_selection_edge.c

Copyright 2020-2021 Daniel T. Borelli <danieltborelli@gmail.com>
Copyright 2021-2023 Guilherme Janczak <guilherme.janczak@yandex.com>
Copyright 2021      Peter Wu <peterwu@hotmail.com>
Copyright 2023-2024 NRK <nrk@disroot.org>

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
#include <X11/extensions/shape.h>

#include "options.h"
#include "scrot.h"
#include "scrot_selection.h"
#include "selection_edge.h"
#include "util.h"

void selectionEdgeCreate(void)
{
    struct SelectionEdge *const pe = &selection.edge;

    XColor color;
    scrotSelectionGetLineColor(&color);

    XSetWindowAttributes attr;
    attr.background_pixel = color.pixel;
    attr.override_redirect = True;

    pe->wndDraw = XCreateWindow(disp, root, 0, 0, WidthOfScreen(scr),
        HeightOfScreen(scr), 0, CopyFromParent, InputOutput, CopyFromParent,
        CWOverrideRedirect | CWBackPixel, &attr);

    unsigned long opacity = opt.lineOpacity * (0xFFFFFFFFu / 255);

    XChangeProperty(disp, pe->wndDraw,
        XInternAtom(disp, "_NET_WM_WINDOW_OPACITY", False), XA_CARDINAL, 32,
        PropModeReplace, (unsigned char *) &opacity, 1L);

    XChangeProperty(disp, pe->wndDraw,
        XInternAtom(disp, "_NET_WM_WINDOW_TYPE", False), XA_ATOM, 32,
        PropModeReplace,
        (unsigned char *) &(Atom) { XInternAtom(disp, "_NET_WM_WINDOW_TYPE_DOCK", False) },
        1L);

    XClassHint hint = { .res_name = "scrot", .res_class = "scrot" };
    XSetClassHint(disp, pe->wndDraw, &hint);
}

void selectionEdgeDraw(void)
{
    const struct Selection *const sel = &selection;
    const struct SelectionEdge *const pe = &sel->edge;

    XRectangle rects[4] = {
        { sel->rect.x, sel->rect.y, opt.lineWidth, sel->rect.h }, // left
        { sel->rect.x, sel->rect.y, sel->rect.w, opt.lineWidth }, // top
        // right
        { sel->rect.x + sel->rect.w, sel->rect.y, opt.lineWidth, sel->rect.h },
        // bottom
        { sel->rect.x, sel->rect.y + sel->rect.h, sel->rect.w + opt.lineWidth,
            opt.lineWidth }
    };

    XShapeCombineRectangles(disp, pe->wndDraw, ShapeBounding, 0, 0, rects, 4,
        ShapeSet, 0);
    XMapWindow(disp, pe->wndDraw);
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

    if (pe->wndDraw != None) {
        XSelectInput(disp, pe->wndDraw, StructureNotifyMask);
        XDestroyWindow(disp, pe->wndDraw);
        bool is_unmapped = false, is_destroyed = false;
        for (XEvent ev; !(is_unmapped && is_destroyed);) {
            XNextEvent(disp, &ev);
            if (ev.type == DestroyNotify && ev.xdestroywindow.window == pe->wndDraw)
                is_destroyed = true;
            if (ev.type == UnmapNotify && ev.xunmap.window == pe->wndDraw)
                is_unmapped = true;
        }
    }
}
