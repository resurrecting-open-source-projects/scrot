/* scrot_selection.h

Copyright 2020-2021 Daniel T. Borelli <daltomi@disroot.org>
Copyright 2021      Martin C <martincation@protonmail.com>
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

#pragma once

#include <assert.h>

#define LINE_MODE_CLASSIC "classic"
#define LINE_MODE_CLASSIC_LEN 7
#define LINE_MODE_EDGE "edge"
#define LINE_MODE_EDGE_LEN 4

enum {
    SELECTION_MODE_CAPTURE = 1,
    SELECTION_MODE_HIDE = 2
};

struct SelectionRect {
    int x, y, w, h;
};

struct SelectionClassic;
struct SelectionEdge;

struct Selection {
    Cursor curCross, curAngleNW, curAngleNE, curAngleSW, curAngleSE;

    struct SelectionRect rect;
    struct SelectionClassic* classic;
    struct SelectionEdge* edge;

    void (*create)(void);
    void (*destroy)(void);
    void (*draw)(void);
    void (*motionDraw)(int, int, int, int);
};

void scrotSelectionCreate(void);
void scrotSelectionDestroy(void);
void scrotSelectionDraw(void);
void scrotSelectionMotionDraw(int, int, int, int);
struct SelectionRect* scrotSelectionGetRect(void);
void scrotSelectionGetLineColor(XColor*);
Status scrotSelectionCreateNamedColor(char const*, XColor*);
void scrotSelectionSetDefaultColorLine(void);
