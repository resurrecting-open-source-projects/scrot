/* scrot_selection_edge.c

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
#include "scrot.h"

extern void selectionCalculateRect(int, int, int, int);
extern struct Selection** selectionGet(void);

struct SelectionEdge {
    Window wndDraw;
    XClassHint* classHint;
};

static Bool xeventUnmap(Display* dpy, XEvent* ev, XPointer arg)
{
    (void)dpy; // unused
    Window* win = (Window*)arg;
    return (ev->xunmap.window == *win);
}

static void waitUnmapWindowNotify(void)
{
    struct Selection const* const sel = *selectionGet();
    struct SelectionEdge const* const pe = sel->edge;
    XEvent ev;

    XSelectInput(disp, pe->wndDraw, StructureNotifyMask);

    XUnmapWindow(disp, pe->wndDraw);

    for (short i = 0; i < 30; ++i) {
        if (XCheckIfEvent(disp, &ev, &xeventUnmap, (XPointer) & (pe->wndDraw)))
            break;
        usleep(8000);
    }
}

void selectionEdgeCreate(void)
{
    struct Selection* const sel = *selectionGet();

    sel->edge = calloc(1, sizeof(*sel->edge));

    struct SelectionEdge* const pe = sel->edge;

    XColor color;
    scrotSelectionGetLineColor(&color);

    XSetWindowAttributes attr;
    attr.background_pixel = color.pixel;
    attr.override_redirect = True;

    pe->classHint = XAllocClassHint();
    pe->classHint->res_name = "scrot";
    pe->classHint->res_class = "scrot";

    pe->wndDraw = XCreateWindow(disp, root, 0, 0, WidthOfScreen(scr), HeightOfScreen(scr), 0,
        CopyFromParent, InputOutput, CopyFromParent, CWOverrideRedirect | CWBackPixel, &attr);

    unsigned long opacity = opt.lineOpacity * ((unsigned)-1 / 100);

    XChangeProperty(disp, pe->wndDraw, XInternAtom(disp, "_NET_WM_WINDOW_OPACITY", False),
        XA_CARDINAL, 32, PropModeReplace,
        (unsigned char*) &opacity, 1L);

    XChangeProperty(disp, pe->wndDraw, XInternAtom(disp, "_NET_WM_WINDOW_TYPE", False),
        XA_ATOM, 32, PropModeReplace,
        (unsigned char*) &(Atom) { XInternAtom(disp, "_NET_WM_WINDOW_TYPE_DOCK", False) },
        1L);

    XSetClassHint(disp, pe->wndDraw, pe->classHint);
}

void selectionEdgeDestroy(void)
{
    struct Selection const* const sel = *selectionGet();
    struct SelectionEdge* pe = sel->edge;

    if (pe->wndDraw != 0) {
        waitUnmapWindowNotify();
        XFree(pe->classHint);
        XDestroyWindow(disp, pe->wndDraw);
    }

    free(pe);
}

void selectionEdgeDraw(void)
{
    struct Selection const* const sel = *selectionGet();
    struct SelectionEdge const* const pe = sel->edge;

    XRectangle rects[4] = {
        { sel->rect.x, sel->rect.y, opt.lineWidth, sel->rect.h }, // left
        { sel->rect.x, sel->rect.y, sel->rect.w, opt.lineWidth }, // top
        { sel->rect.x + sel->rect.w, sel->rect.y, opt.lineWidth, sel->rect.h }, // right
        { sel->rect.x, sel->rect.y + sel->rect.h, sel->rect.w + opt.lineWidth, opt.lineWidth } // bottom
    };

    XShapeCombineRectangles(disp, pe->wndDraw, ShapeBounding, 0, 0, rects, 4, ShapeSet, 0);
    XMapWindow(disp, pe->wndDraw);
}

void selectionEdgeMotionDraw(int x0, int y0, int x1, int y1)
{
    struct Selection* const sel = *selectionGet();

    selectionCalculateRect(x0, y0, x1, y1);

    sel->rect.x -= opt.lineWidth;
    sel->rect.y -= opt.lineWidth;
    sel->rect.w += opt.lineWidth;
    sel->rect.h += opt.lineWidth;

    selectionEdgeDraw();
}
