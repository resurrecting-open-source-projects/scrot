/* scrot_selection.c

Copyright 2020-2022  Daniel T. Borelli <danieltborelli@gmail.com>
Copyright 2021       Martin C <martincation@protonmail.com>
Copyright 2021       Peter Wu <peterwu@hotmail.com>
Copyright 2021       Wilson Smith <01wsmith+gh@gmail.com>
Copyright 2021-2023  Guilherme Janczak <guilherme.janczak@yandex.com>
Copyright 2022       NRK <nrk@disroot.org>

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

#include <sys/select.h>

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <Imlib2.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>

#include "imlib.h"
#include "options.h"
#include "scrot.h"
#include "scrot_selection.h"
#include "selection_classic.h"
#include "selection_edge.h"

struct Selection **selectionGet(void)
{
    static struct Selection *sel = NULL;
    return &sel;
}

static void selectionAllocate(void)
{
    struct Selection **sel = selectionGet();
    *sel = calloc(1, sizeof(**sel));
}

static void selectionDeallocate(void)
{
    struct Selection **sel = selectionGet();
    free(*sel);
    *sel = NULL;
}

static void createCursors(void)
{
    struct Selection *const sel = *selectionGet();

    assert(sel != NULL);

    if (opt.selection.mode == SELECTION_MODE_CAPTURE)
        sel->curCross = XCreateFontCursor(disp, XC_cross);
    else if (opt.selection.mode == SELECTION_MODE_HIDE)
        sel->curCross = XCreateFontCursor(disp, XC_spraycan);
    else if (opt.selection.mode == SELECTION_MODE_BLUR)
        sel->curCross = XCreateFontCursor(disp, XC_box_spiral);
    else // SELECTION_MODE_HOLE
        sel->curCross = XCreateFontCursor(disp, XC_target);
    sel->curAngleNE = XCreateFontCursor(disp, XC_ur_angle);
    sel->curAngleNW = XCreateFontCursor(disp, XC_ul_angle);
    sel->curAngleSE = XCreateFontCursor(disp, XC_lr_angle);
    sel->curAngleSW = XCreateFontCursor(disp, XC_ll_angle);
}

static void freeCursors(void)
{
    struct Selection *const sel = *selectionGet();

    assert(sel != NULL);

    XFreeCursor(disp, sel->curCross);
    XFreeCursor(disp, sel->curAngleNE);
    XFreeCursor(disp, sel->curAngleNW);
    XFreeCursor(disp, sel->curAngleSE);
    XFreeCursor(disp, sel->curAngleSW);
}

void selectionCalculateRect(int x0, int y0, int x1, int y1)
{
    struct SelectionRect *const rect = scrotSelectionGetRect();

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

    struct Selection *const sel = *selectionGet();

    assert(sel != NULL);

    createCursors();

    if (!strncmp(opt.lineMode, LINE_MODE_S_CLASSIC, LINE_MODE_L_CLASSIC)) {
        sel->create = selectionClassicCreate;
        sel->draw = selectionClassicDraw;
        sel->motionDraw = selectionClassicMotionDraw;
        sel->destroy = selectionClassicDestroy;
    } else if (!strncmp(opt.lineMode, LINE_MODE_S_EDGE, LINE_MODE_L_EDGE)) {
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
    struct Selection *const sel = *selectionGet();
    XUngrabPointer(disp, CurrentTime);
    freeCursors();
    XSync(disp, True);
    sel->destroy();
    selectionDeallocate();
}

void scrotSelectionDraw(void)
{
    const struct Selection *const sel = *selectionGet();
    sel->draw();
}

void scrotSelectionMotionDraw(int x0, int y0, int x1, int y1)
{
    const struct Selection *const sel = *selectionGet();
    const unsigned int EVENT_MASK = ButtonMotionMask | ButtonReleaseMask;
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

struct SelectionRect *scrotSelectionGetRect(void)
{
    return &(*selectionGet())->rect;
}

Status scrotSelectionCreateNamedColor(const char *nameColor, XColor *color)
{
    assert(nameColor != NULL);
    assert(color != NULL);

    return XAllocNamedColor(disp, XDefaultColormap(disp, DefaultScreen(disp)),
        nameColor, color, &(XColor){0});
}

void scrotSelectionGetLineColor(XColor *color)
{
    scrotSelectionSetDefaultColorLine();

    const Status ret = scrotSelectionCreateNamedColor(opt.lineColor, color);

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

bool scrotSelectionGetUserSel(struct SelectionRect *selectionRect)
{
    static int xfd = 0;
    static int fdSize = 0;
    XEvent ev;
    fd_set fdSet;
    int count = 0, done = 0;
    int rx = 0, ry = 0, rw = 0, rh = 0, isButtonPressed = 0;
    Window target = None;
    Status ret;

    scrotSelectionCreate();

    xfd = ConnectionNumber(disp);
    fdSize = xfd + 1;

    ret = XGrabKeyboard(disp, root, False, GrabModeAsync, GrabModeAsync, CurrentTime);
    if (ret == AlreadyGrabbed) {
        int attempts = 20;
        struct timespec delay = { 0, 50 * 1000L * 1000L };
        do {
            nanosleep(&delay, NULL);
            ret = XGrabKeyboard(disp, root, False, GrabModeAsync, GrabModeAsync, CurrentTime);
        } while (--attempts > 0 && ret == AlreadyGrabbed);
    }
    if (ret != GrabSuccess) {
        scrotSelectionDestroy();
        errx(EXIT_FAILURE, "failed to grab keyboard");
    }

    while (1) {
        /* Handle events here */
        while (!done && XPending(disp)) {
            XNextEvent(disp, &ev);
            switch (ev.type) {
            case MotionNotify:
                if (isButtonPressed)
                    scrotSelectionMotionDraw(rx, ry, ev.xbutton.x, ev.xbutton.y);
                break;
            case ButtonPress:
                isButtonPressed = 1;
                rx = ev.xbutton.x;
                ry = ev.xbutton.y;
                target = scrotGetWindow(disp, ev.xbutton.subwindow, ev.xbutton.x, ev.xbutton.y);
                if (target == None)
                    target = root;
                break;
            case ButtonRelease:
                done = 1;
                break;
            case KeyPress:
            {
                KeySym *keysym = NULL;
                int keycode; /*dummy*/

                keysym = XGetKeyboardMapping(disp, ev.xkey.keycode, 1, &keycode);

                if (!keysym)
                    break;

                if (!isButtonPressed) {
                key_abort_shot:
                    if (!opt.ignoreKeyboard || *keysym == XK_Escape) {
                        warnx("Key was pressed, aborting shot");
                        done = 2;
                    }
                    XFree(keysym);
                    break;
                }

                switch (*keysym) {
                case XK_Right:
                    if (++rx > scr->width)
                        rx = scr->width;
                    break;
                case XK_Left:
                    if (--rx < 0)
                        rx = 0;
                    break;
                case XK_Down:
                    if (++ry > scr->height)
                        ry = scr->height;
                    break;
                case XK_Up:
                    if (--ry < 0)
                        ry = 0;
                    break;
                default:
                    goto key_abort_shot;
                }
                XFree(keysym);
                scrotSelectionMotionDraw(rx, ry, ev.xbutton.x, ev.xbutton.y);
                break;
            }
            case KeyRelease:
                /* ignore */
                break;
            default:
                break;
            }
        }
        if (done)
            break;

        /* now block some */
        FD_ZERO(&fdSet);
        FD_SET(xfd, &fdSet);
        errno = 0;
        count = select(fdSize, &fdSet, NULL, NULL, NULL);
        if ((count < 0)
            && ((errno == ENOMEM) || (errno == EINVAL) || (errno == EBADF))) {
            scrotSelectionDestroy();
            errx(EXIT_FAILURE, "Connection to X display lost");
        }
    }
    scrotSelectionDraw();

    XUngrabKeyboard(disp, CurrentTime);

    const bool isAreaSelect = (scrotSelectionGetRect()->w > 5);

    scrotSelectionDestroy();

    if (done == 2)
        return false;

    if (isAreaSelect) {
        /* If a rect has been drawn, it's an area selection */
        rw = ev.xbutton.x - rx;
        rh = ev.xbutton.y - ry;

        if ((ev.xbutton.x + 1) == WidthOfScreen(scr))
            ++rw;

        if ((ev.xbutton.y + 1) == HeightOfScreen(scr))
            ++rh;

        if (rw < 0) {
            rx += rw;
            rw = 0 - rw;
        }

        if (rh < 0) {
            ry += rh;
            rh = 0 - rh;
        }

        // Not record pointer if there is a selection area because it is busy on that,
        // unless the delay option is used.
        if (opt.delay == 0)
            opt.pointer = 0;
    } else {
        /* else it's a window click */
        if (!scrotGetGeometry(target, &rx, &ry, &rw, &rh))
            return false;
        clientWindow = target;
    }
    scrotNiceClip(&rx, &ry, &rw, &rh);

    if (!opt.silent)
        XBell(disp, 0);

    selectionRect->x = rx;
    selectionRect->y = ry;
    selectionRect->w = rw;
    selectionRect->h = rh;
    return true;
}

static void changeImageOpacity(Imlib_Image image, const int opacity)
{
    imlib_context_set_image(image);
    const int w = imlib_image_get_width();
    const int h = imlib_image_get_height();

    uint32_t *data = imlib_image_get_data();
    uint32_t *end = data + (h * w);

    for (uint32_t *pixel = data; pixel != end; ++pixel) {
        const uint32_t a = (*pixel >> 24) * opacity / 255;
       *pixel = (a << 24) | (*pixel & 0x00FFFFFF);
    }

    imlib_image_put_back_data(data);
}

static Imlib_Image loadImage(const char *const fileName, const int opacity)
{
    Imlib_Image image = imlib_load_image(fileName);

    if (!image) {
        errx(EXIT_FAILURE, "option --select: Failed to load image:%s",
            fileName);
    }

    imlib_context_set_image(image);

    if (imlib_image_has_alpha() == 0) {
        warnx("Warning, ignoring the opacity parameter because the image '%s'"
            " has no alpha channel, it will be drawn fully opaque.", fileName);
        return image;
    }

    if (opacity == 255) {
        // Do nothing if a totally opaque image is expected.
        return image;
    }

    changeImageOpacity(image, opacity);

    return image;
}

Imlib_Image scrotSelectionSelectMode(void)
{
    struct SelectionRect rect0, rect1;

    const unsigned int oldMode = opt.selection.mode;

    opt.selection.mode = SELECTION_MODE_CAPTURE;

    if (!scrotSelectionGetUserSel(&rect0))
        return NULL;

    opt.selection.mode = oldMode;

    if (opt.selection.mode & SELECTION_MODE_NOT_CAPTURE)
        if (!scrotSelectionGetUserSel(&rect1))
            return NULL;

    scrotDoDelay();

    Imlib_Image capture = imlib_create_image_from_drawable(0, rect0.x, rect0.y,
        rect0.w, rect0.h, 1);

    if (opt.pointer)
       scrotGrabMousePointer(capture, rect0.x, rect0.y);

    if (opt.selection.mode == SELECTION_MODE_CAPTURE)
        return capture;

    XColor color;
    scrotSelectionGetLineColor(&color);

    const int x = rect1.x - rect0.x;
    const int y = rect1.y - rect0.y;
    const int opacity = opt.lineOpacity;

    imlib_context_set_image(capture);

    switch(opt.selection.mode) {
    case SELECTION_MODE_HOLE:
        if (opacity > 0) {
            Imlib_Image hole = imlib_clone_image();
            imlib_context_set_color(color.red, color.green, color.blue, opacity);
            imlib_image_fill_rectangle(0, 0, rect0.w, rect0.h);
            imlib_blend_image_onto_image(hole, 0, x, y, rect1.w, rect1.h, x, y, rect1.w, rect1.h);
            imlib_context_set_image(hole);
            imlib_free_image_and_decache();
        }
        break;
    case SELECTION_MODE_HIDE:
    {
        char *const fileName = opt.selection.paramStr;

        if (fileName) {
            if (opacity > 0) {
                Imlib_Image hide = loadImage(fileName, opacity);
                imlib_context_set_image(hide);
                int const w = imlib_image_get_width();
                int const h = imlib_image_get_height();
                imlib_context_set_image(capture);
                imlib_blend_image_onto_image(hide, 0, 0, 0, w, h, x, y, rect1.w, rect1.h);
                imlib_context_set_image(hide);
                imlib_free_image_and_decache();
            }
            free(fileName);
        } else {
            imlib_context_set_color(color.red, color.green, color.blue, opacity);
            imlib_image_fill_rectangle(x, y, rect1.w, rect1.h);
        }
        break;
    }
    case SELECTION_MODE_BLUR:
    {
        const int amountBlur = opt.selection.paramNum;
        Imlib_Image blur = imlib_clone_image();
        imlib_context_set_image(blur);
        imlib_image_blur(amountBlur);
        imlib_context_set_image(capture);
        imlib_blend_image_onto_image(blur, 0, x, y, rect1.w, rect1.h, x, y, rect1.w, rect1.h);
        imlib_context_set_image(blur);
        imlib_free_image_and_decache();
        break;
    }
    default:
        assert(0);
    }

    return capture;
}
