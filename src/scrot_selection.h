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

#include <X11/Xresource.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/shape.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#include <Imlib2.h>
#include <stdbool.h>
#include <assert.h>

/* S: string, L: len */
#define LINE_MODE_S_CLASSIC "classic"
#define LINE_MODE_L_CLASSIC 7
#define LINE_MODE_S_EDGE "edge"
#define LINE_MODE_L_EDGE 4
#define SELECTION_MODE_S_CAPTURE "capture"
#define SELECTION_MODE_L_CAPTURE 7
#define SELECTION_MODE_S_HIDE "hide"
#define SELECTION_MODE_L_HIDE 4
#define SELECTION_MODE_S_HOLE "hole"
#define SELECTION_MODE_L_HOLE 4
#define SELECTION_MODE_S_BLUR "blur"
#define SELECTION_MODE_L_BLUR 4
#define SELECTION_MODE_S_IMAGE "image" //TODO: soon
#define SELECTION_MODE_L_IMAGE 5

enum {
    SELECTION_MODE_SEPARATOR = '@',
    SELECTION_MODE_CAPTURE = (1 << 1),
    SELECTION_MODE_HIDE = (1 << 2),
    SELECTION_MODE_HOLE = (1 << 3),
    SELECTION_MODE_BLUR = (1 << 4),
    SELECTION_MODE_IMAGE = (1 << 5),
    SELECTION_MODE_ANY = (SELECTION_MODE_CAPTURE | SELECTION_MODE_HIDE | SELECTION_MODE_HOLE | SELECTION_MODE_BLUR | SELECTION_MODE_IMAGE),
    SELECTION_MODE_PARAM_NUM = (SELECTION_MODE_HIDE | SELECTION_MODE_HOLE | SELECTION_MODE_BLUR),
    SELECTION_MODE_PARAM_STR = (SELECTION_MODE_IMAGE),
    SELECTION_MODE_NOT_CAPTURE = (SELECTION_MODE_ANY & ~SELECTION_MODE_CAPTURE),
    SELECTION_MODE_NOT_BLUR = (SELECTION_MODE_NOT_CAPTURE & ~SELECTION_MODE_BLUR),
    SELECTION_MODE_BLUR_DEFAULT = 18, /* range 1..30 */
    SELECTION_MODE_OPACITY_DEFAULT = 100, /* range 0..100 */
};

struct SelectionRect {
    int x, y, w, h;
};

typedef struct SelectionMode {
    unsigned int mode;
    int paramNum;
    char* paramStr;
} SelectionMode;

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
bool scrotSelectionGetUserSel(struct SelectionRect*);
Imlib_Image scrotSelectionSelectMode(void);
