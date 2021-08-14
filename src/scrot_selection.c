/* scrot_selection.c

Copyright 2020-2021  Daniel T. Borelli <daltomi@disroot.org>
Copyright 2021       Martin C <martincation@protonmail.com>
Copyright 2021       Peter Wu <peterwu@hotmail.com>

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

struct Selection** selectionGet(void)
{
    static struct Selection* sel = NULL;
    return &sel;
}

static void selectionAllocate(void)
{
    struct Selection** sel = selectionGet();
    *sel = calloc(1, sizeof(**sel));
}

static void selectionDeallocate(void)
{
    struct Selection** sel = selectionGet();
    free(*sel);
    *sel = NULL;
}

static void createCursors(void)
{
    struct Selection* const sel = *selectionGet();

    assert(sel != NULL);

    if (opt.select == SELECTION_MODE_HIDE)
        sel->curCross = XCreateFontCursor(disp, XC_target);
    else
        sel->curCross = XCreateFontCursor(disp, XC_cross);
    sel->curAngleNE = XCreateFontCursor(disp, XC_ur_angle);
    sel->curAngleNW = XCreateFontCursor(disp, XC_ul_angle);
    sel->curAngleSE = XCreateFontCursor(disp, XC_lr_angle);
    sel->curAngleSW = XCreateFontCursor(disp, XC_ll_angle);
}

static void freeCursors(void)
{
    struct Selection* const sel = *selectionGet();

    assert(sel != NULL);

    XFreeCursor(disp, sel->curCross);
    XFreeCursor(disp, sel->curAngleNE);
    XFreeCursor(disp, sel->curAngleNW);
    XFreeCursor(disp, sel->curAngleSE);
    XFreeCursor(disp, sel->curAngleSW);
}

void selectionCalculateRect(int x0, int y0, int x1, int y1)
{
    struct SelectionRect* const rect = scrotSelectionGetRect();

    rect->x = x0;
    rect->y = y0;
    rect->w = x1 - x0;
    rect->h = y1 - y0;

    if (rect->w == 0)
        rect->w++;

    if (rect->h == 0)
        rect->h++;

    if (rect->w < 0) {
        rect->x += rect->w;
        rect->w = 0 - rect->w;
    }

    if (rect->h < 0) {
        rect->y += rect->h;
        rect->h = 0 - rect->h;
    }
}

void scrotSelectionCreate(void)
{
    selectionAllocate();

    struct Selection* const sel = *selectionGet();

    assert(sel != NULL);

    createCursors();

    if (!strncmp(opt.lineMode, LINE_MODE_CLASSIC, LINE_MODE_CLASSIC_LEN)) {
        sel->create = selectionClassicCreate;
        sel->draw = selectionClassicDraw;
        sel->motionDraw = selectionClassicMotionDraw;
        sel->destroy = selectionClassicDestroy;
    } else if (!strncmp(opt.lineMode, LINE_MODE_EDGE, LINE_MODE_EDGE_LEN)) {
        sel->create = selectionEdgeCreate;
        sel->draw = selectionEdgeDraw;
        sel->motionDraw = selectionEdgeMotionDraw;
        sel->destroy = selectionEdgeDestroy;
    } else {
        // It never happened, fix the options.c file
        assert(0);
    }

    sel->create();

    unsigned int const EVENT_MASK = ButtonMotionMask | ButtonPressMask | ButtonReleaseMask;

    if ((XGrabPointer(disp, root, False, EVENT_MASK, GrabModeAsync,
             GrabModeAsync, root, sel->curCross, CurrentTime)
            != GrabSuccess)) {
        scrotSelectionDestroy();
        errx(EXIT_FAILURE, "couldn't grab pointer");
    }
}

void scrotSelectionDestroy(void)
{
    struct Selection* const sel = *selectionGet();
    XUngrabPointer(disp, CurrentTime);
    freeCursors();
    XSync(disp, True);
    sel->destroy();
    selectionDeallocate();
}

void scrotSelectionDraw(void)
{
    struct Selection const* const sel = *selectionGet();
    sel->draw();
}

void scrotSelectionMotionDraw(int x0, int y0, int x1, int y1)
{
    struct Selection const* const sel = *selectionGet();
    unsigned int const EVENT_MASK = ButtonMotionMask | ButtonReleaseMask;
    Cursor cursor = None;

    if (x1 > x0 && y1 > y0)
        cursor = sel->curAngleSE;
    else if (x1 > x0)
        cursor = sel->curAngleNE;
    else if (y1 > y0)
        cursor = sel->curAngleSW;
    else
        cursor = sel->curAngleNW;
    XChangeActivePointerGrab(disp, EVENT_MASK, cursor, CurrentTime);
    sel->motionDraw(x0, y0, x1, y1);
}

struct SelectionRect* scrotSelectionGetRect(void)
{
    return &(*selectionGet())->rect;
}

Status scrotSelectionCreateNamedColor(char* nameColor, XColor* color)
{
    assert(nameColor != NULL);
    assert(color != NULL);

    return XAllocNamedColor(disp, XDefaultColormap(disp, DefaultScreen(disp)),
        nameColor, color, &(XColor) {});
}

void scrotSelectionGetLineColor(XColor* color)
{
    scrotSelectionSetDefaultColorLine();

    Status const ret = scrotSelectionCreateNamedColor(opt.lineColor, color);

    if (!ret) {
        scrotSelectionDestroy();
        errx(EXIT_FAILURE, "Error allocate color:%s", strerror(BadColor));
    }
}

void scrotSelectionSetDefaultColorLine(void)
{
    if (!opt.lineColor)
        opt.lineColor = "gray";
}
