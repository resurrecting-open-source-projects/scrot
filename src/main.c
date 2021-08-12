/* main.c

Copyright 1999-2000 Tom Gilbert <tom@linuxbrit.co.uk,
                                  gilbertt@linuxbrit.co.uk,
                                  scrot_sucks@linuxbrit.co.uk>
Copyright 2009      James Cameron <quozl@us.netrek.org>
Copyright 2010      Ibragimov Rinat <ibragimovrinat@mail.ru>
Copyright 2017      Stoney Sauce <stoneysauce@gmail.com>
Copyright 2019-2021 Daniel T. Borelli <daltomi@disroot.org>
Copyright 2019      Jade Auer <jade@trashwitch.dev>
Copyright 2020      blockparole
Copyright 2020      Cungsten Tarbide <ctarbide@tuta.io>
Copyright 2020      Hinigatsu <hinigatsu@protonmail.com>
Copyright 2020      nothub
Copyright 2020      Sean Brennan <zettix1@gmail.com>
Copyright 2021      c0dev0id <sh+github@codevoid.de>
Copyright 2021      Christopher R. Nelson <christopher.nelson@languidnights.com>
Copyright 2021      Guilherme Janczak <guilherme.janczak@yandex.com>
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

#include "options.h"
#include "scrot.h"
#include "slist.h"
#include <assert.h>

/* atexit register func. */
static void uninitXAndImlib(void)
{
    if (disp) {
        XCloseDisplay(disp);
        disp = NULL;
    }
}

// It assumes that the local variable 'main.c:Imlib_Image image' is in context
static void applyFilterIfRequired(void)
{
    if (opt.script)
        imlib_apply_filter(opt.script);
}

int main(int argc, char** argv)
{
    Imlib_Image image;
    Imlib_Image thumbnail;
    Imlib_Load_Error imErr;
    char* filenameIM = NULL;
    char* filenameThumb = NULL;

    char* haveExtension = NULL;

    time_t t;
    struct tm* tm;

    optionsParse(argc, argv);

    initXAndImlib(opt.display, 0);

    atexit(uninitXAndImlib);

    if (!opt.outputFile) {
        opt.outputFile = strdup("%Y-%m-%d-%H%M%S_$wx$h_scrot.png");
        opt.thumbFile = strdup("%Y-%m-%d-%H%M%S_$wx$h_scrot-thumb.png");
    } else
        scrotHaveFileExtension(opt.outputFile, &haveExtension);

    if (opt.focused)
        image = scrotGrabFocused();
    else if (opt.select)
        image = scrotSelAndGrabImage();
    else if (opt.autoselect)
        image = scrotGrabAutoselect();
    else {
        scrotDoDelay();
        if (opt.multidisp)
            image = scrotGrabShotMulti();
        else if (opt.stack)
            image = scrotGrabStackWindows();
        else
            image = scrotGrabShot();
    }

    if (opt.note)
        scrotNoteDraw(image);

    if (!image) {
        fprintf(stderr, "no image grabbed: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    time(&t); /* Get the time directly after the screenshot */
    tm = localtime(&t);

    imlib_context_set_image(image);
    imlib_image_attach_data_value("quality", NULL, opt.quality, NULL);

    if (!haveExtension)
        imlib_image_set_format("png");

    filenameIM = imPrintf(opt.outputFile, tm, NULL, NULL, image);
    scrotCheckIfOverwriteFile(&filenameIM);

    applyFilterIfRequired();

    imlib_save_image_with_error_return(filenameIM, &imErr);
    if (imErr) {
        fprintf(stderr, "Saving to file %s failed: %s\n", filenameIM, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (opt.thumb) {
        int cwidth, cheight;
        int twidth, theight;

        cwidth = imlib_image_get_width();
        cheight = imlib_image_get_height();

        /* Geometry based thumb size */
        if (opt.thumbWidth || opt.thumbHeight) {
            if (!opt.thumbWidth) {
                twidth = cwidth * opt.thumbHeight / cheight;
                theight = opt.thumbHeight;
            } else if (!opt.thumbHeight) {
                twidth = opt.thumbWidth;
                theight = cheight * opt.thumbWidth / cwidth;
            } else {
                twidth = opt.thumbWidth;
                theight = opt.thumbHeight;
            }
        } else {
            twidth = cwidth * opt.thumb / 100;
            theight = cheight * opt.thumb / 100;
        }

        imlib_context_set_anti_alias(1);
        thumbnail = imlib_create_cropped_scaled_image(0, 0, cwidth, cheight,
            twidth, theight);
        if (!thumbnail)
            err(EXIT_FAILURE, "unable to create thumbnail");
        else {
            if (opt.note)
                scrotNoteDraw(image);

            scrotHaveFileExtension(opt.thumbFile, &haveExtension);

            if (!haveExtension) {
                imlib_context_set_image(thumbnail);
                imlib_image_set_format("png");
            }

            filenameThumb = imPrintf(opt.thumbFile, tm, NULL, NULL, thumbnail);
            scrotCheckIfOverwriteFile(&filenameThumb);

            applyFilterIfRequired();

            imlib_save_image_with_error_return(filenameThumb, &imErr);
            if (imErr) {
                fprintf(stderr, "Saving thumbnail %s failed: %s\n", filenameThumb, strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
    }
    if (opt.exec)
        scrotExecApp(image, tm, filenameIM, filenameThumb);
    imlib_free_image_and_decache();

    return 0;
}

void scrotDoDelay(void)
{
    if (opt.delay) {
        if (opt.countdown) {
            int i;

            printf("Taking shot in %d.. ", opt.delay);
            fflush(stdout);
            sleep(1);
            for (i = opt.delay - 1; i > 0; i--) {
                printf("%d.. ", i);
                fflush(stdout);
                sleep(1);
            }
            printf("0.\n");
            fflush(stdout);
        } else
            sleep(opt.delay);
    }
}

size_t scrotHaveFileExtension(char const* filename, char** ext)
{
    *ext = strrchr(filename, '.');

    if (*ext)
        return strlen(*ext);

    return 0;
}

void scrotCheckIfOverwriteFile(char** filename)
{
    char* curFile = *filename;

    if (opt.overwrite)
        return;

    if (access(curFile, F_OK) == -1)
        return;

    const size_t maxCounter = 999;
    size_t counter = 0;
    char* ext = NULL;
    size_t extLength = 0;
    const size_t slen = strlen(curFile);
    size_t nalloc = slen + 4 + 1; // _000 + NUL byte
    char fmt[5];
    char* newName = NULL;

    extLength = scrotHaveFileExtension(curFile, &ext);

    if (ext)
        nalloc += extLength; // .ext

    newName = calloc(nalloc, sizeof(char));

    if (ext) {
        // Exclude ext
        memcpy(newName, curFile, slen - extLength);
    } else
        memcpy(newName, curFile, slen);

    do {
        snprintf(fmt, 5, "_%03zu", counter++);

        if (!ext)
            strlcpy(newName + slen, fmt, 5);
        else {
            strncpy((newName + slen) - extLength, fmt, 5);
            strncat(newName, ext, extLength);
        }
        curFile = newName;
    } while ((counter < maxCounter) && !access(curFile, F_OK));

    free(*filename);
    *filename = newName;

    if (counter == maxCounter) {
        fprintf(stderr, "scrot can no longer generate new file names.\n"
                        "The last attempt is %s\n",
            newName);
        free(newName);
        exit(EXIT_FAILURE);
    }
}

int scrotMatchWindowClassName(Window target)
{
    assert(disp != NULL);

    const int NOT_MATCH = 0;
    const int MATCH = 1;
    /* By default all class names match since windowClassName by default is NULL*/
    int retval = MATCH;

    if (!opt.windowClassName)
        return retval;

    XClassHint classHint;
    retval = NOT_MATCH; // windowClassName != NULL, by default NOT_MATCH

    if (XGetClassHint(disp, target, &classHint) != BadWindow) {
        retval = optionsCompareWindowClassName(classHint.res_class);
        XFree(classHint.res_name);
        XFree(classHint.res_class);
    }

    return retval;
}

void scrotGrabMousePointer(const Imlib_Image image,
    const int xOffset, const int yOffset)
{
    XFixesCursorImage* xcim = XFixesGetCursorImage(disp);

    const unsigned short width = xcim->width;
    const unsigned short height = xcim->height;
    const int x = (xcim->x - xcim->xhot) - xOffset;
    const int y = (xcim->y - xcim->yhot) - yOffset;
    DATA32* pixels = NULL;

#ifdef __i386__
    pixels = (DATA32*)xcim->pixels;
#else
    DATA32 data[width * height * 4];

    unsigned int i;
    for (i = 0; i < (width * height); i++)
        data[i] = (DATA32)xcim->pixels[i];

    pixels = data;
#endif

    Imlib_Image imcursor = imlib_create_image_using_data(width, height, pixels);

    XFree(xcim);

    if (!imcursor) {
        fprintf(stderr, "scrotGrabMousePointer: Failed create image using data.\n");
        exit(EXIT_FAILURE);
    }

    imlib_context_set_image(imcursor);
    imlib_image_set_has_alpha(1);
    imlib_context_set_image(image);
    imlib_blend_image_onto_image(imcursor, 0, 0, 0, width, height, x, y, width, height);
    imlib_context_set_image(imcursor);
    imlib_free_image();
}

Imlib_Image scrotGrabShot(void)
{
    Imlib_Image im;

    if (!opt.silent)
        XBell(disp, 0);

    im = imlib_create_image_from_drawable(0, 0, 0, scr->width,
        scr->height, 1);
    if (opt.pointer)
        scrotGrabMousePointer(im, 0, 0);

    return im;
}

void scrotExecApp(Imlib_Image image, struct tm* tm,
    char* filenameIM, char* filenameThumb)
{
    char* execStr;
    int ret;

    execStr = imPrintf(opt.exec, tm, filenameIM, filenameThumb, image);

    errno = 0;

    ret = system(execStr);

    if (ret == -1)
        fprintf(stderr, "The child process could not be created: %s\n", strerror(errno));
    else if (WEXITSTATUS(ret) == 127)
        fprintf(stderr, "scrot could not execute the command: %s.\n", execStr);

    exit(0);
}

Imlib_Image scrotGrabFocused(void)
{
    Imlib_Image im = NULL;
    int rx = 0, ry = 0, rw = 0, rh = 0;
    Window target = None;
    int ignored;

    scrotDoDelay();
    XGetInputFocus(disp, &target, &ignored);
    if (!scrotGetGeometry(target, &rx, &ry, &rw, &rh))
        return NULL;
    scrotNiceClip(&rx, &ry, &rw, &rh);
    im = imlib_create_image_from_drawable(0, rx, ry, rw, rh, 1);
    if (opt.pointer)
        scrotGrabMousePointer(im, rx, ry);
    return im;
}

Imlib_Image scrotSelAndGrabImage(void)
{
    Imlib_Image im = NULL;
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
        fprintf(stderr, "failed to grab keyboard\n");
        scrotSelectionDestroy();
        exit(EXIT_FAILURE);
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
                if (!isButtonPressed) {
                key_abort_shot:
                    fprintf(stderr, "Key was pressed, aborting shot\n");
                    done = 2;
                    break;
                }

                KeySym* keysym = NULL;
                int keycode; /*dummy*/

                keysym = XGetKeyboardMapping(disp, ev.xkey.keycode, 1, &keycode);

                if (!keysym)
                    break;

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
            fprintf(stderr, "Connection to X display lost\n");
            scrotSelectionDestroy();
            exit(EXIT_FAILURE);
        }
    }
    scrotSelectionDraw();

    XUngrabKeyboard(disp, CurrentTime);

    bool const isAreaSelect = (scrotSelectionGetRect()->w > 5);

    scrotSelectionDestroy();

    if (done < 2) {
        scrotDoDelay();
        if (isAreaSelect) {
            /* If a rect has been drawn, it's an area selection */
            rw = ev.xbutton.x - rx;
            rh = ev.xbutton.y - ry;

            if ((ev.xbutton.x + 1) == WidthOfScreen(scr)) {
                ++rw;
            }

            if ((ev.xbutton.y + 1) == HeightOfScreen(scr)) {
                ++rh;
            }

            if (rw < 0) {
                rx += rw;
                rw = 0 - rw;
            }
            if (rh < 0) {
                ry += rh;
                rh = 0 - rh;
            }

            // Not record pointer if there is a selection area because it is busy on that,
            // unless the delay option is used
            if (opt.delay == 0)
                opt.pointer = 0;
        } else {
            /* else it's a window click */
            if (!scrotGetGeometry(target, &rx, &ry, &rw, &rh))
                return NULL;
        }
        scrotNiceClip(&rx, &ry, &rw, &rh);

        if (!opt.silent)
            XBell(disp, 0);
        im = imlib_create_image_from_drawable(0, rx, ry, rw, rh, 1);

        if (opt.pointer)
            scrotGrabMousePointer(im, rx, ry);
    }
    return im;
}

Imlib_Image scrotGrabAutoselect(void)
{
    Imlib_Image im = NULL;
    int rx = opt.autoselectX, ry = opt.autoselectY, rw = opt.autoselectW, rh = opt.autoselectH;

    scrotDoDelay();
    scrotNiceClip(&rx, &ry, &rw, &rh);
    im = imlib_create_image_from_drawable(0, rx, ry, rw, rh, 1);
    if (opt.pointer)
        scrotGrabMousePointer(im, rx, ry);
    return im;
}

/* Clip rectangle nicely */
void scrotNiceClip(int* rx, int* ry, int* rw, int* rh)
{
    if (*rx < 0) {
        *rw += *rx;
        *rx = 0;
    }
    if (*ry < 0) {
        *rh += *ry;
        *ry = 0;
    }
    if ((*rx + *rw) > scr->width)
        *rw = scr->width - *rx;
    if ((*ry + *rh) > scr->height)
        *rh = scr->height - *ry;
}

static Bool scrotXEventVisibility(Display* dpy, XEvent* ev, XPointer arg)
{
    (void)dpy; // unused
    Window* win = (Window*)arg;
    return (ev->xvisibility.window == *win);
}

/* Get geometry of window and use that */
int scrotGetGeometry(Window target, int* rx, int* ry, int* rw, int* rh)
{
    Window child;
    XWindowAttributes attr;
    int stat, frames = 0;

    /* Get windowmanager frame of window */
    if (target != root) {
        unsigned int d;
        int x;
        int status;

        status = XGetGeometry(disp, target, &root, &x, &x, &d, &d, &d, &d);
        if (status) {
            Window rt, *children, parent;
            XEvent ev;

            for (;;) {
                /* Find window manager frame. */
                status = XQueryTree(disp, target, &rt, &parent, &children, &d);
                if (status && (children != None))
                    XFree(children);
                if (!status || (parent == None) || (parent == rt))
                    break;
                target = parent;
                ++frames;
            }
            /* Get client window. */
            if (!opt.border)
                target = scrotGetClientWindow(disp, target);
            XRaiseWindow(disp, target);

            /* Give the WM time to update the hidden area of the window.
               Some windows never send the event, a time limit is placed.
            */
            XSelectInput(disp, target, FocusChangeMask);
            for (short i = 0; i < 30; ++i) {
                if (XCheckIfEvent(disp, &ev, &scrotXEventVisibility, (XPointer)&target))
                    break;
                usleep(2000);
            }
        }
    }
    stat = XGetWindowAttributes(disp, target, &attr);
    if (!stat || (attr.map_state != IsViewable))
        return 0;
    *rw = attr.width;
    *rh = attr.height;
    XTranslateCoordinates(disp, target, root, 0, 0, rx, ry, &child);

    /* Special case when the TWM emulates the border directly on the window. */
    if (opt.border == 1 && frames < 2 && attr.border_width > 0) {
        *rw += attr.border_width * 2;
        *rh += attr.border_width * 2;
        *rx -= attr.border_width;
        *ry -= attr.border_width;
    }
    return 1;
}

Window scrotGetWindow(Display* display, Window window, int x, int y)
{
    Window source, target;

    int status, xOffset, yOffset;

    source = root;
    target = window;
    if (window == None)
        window = root;
    while (1) {
        status = XTranslateCoordinates(display, source, window, x, y, &xOffset,
            &yOffset, &target);
        if (!status)
            break;
        if (target == None)
            break;
        source = window;
        window = target;
        x = xOffset;
        y = yOffset;
    }
    if (target == None)
        target = window;
    return target;
}

char* imPrintf(char* str, struct tm* tm, char* filenameIM, char* filenameThumb, Imlib_Image im)
{
    char* c;
    char buf[20];
    char ret[4096];
    char strf[4096];
    char* tmp;
    struct stat st;

    ret[0] = '\0';

    if (strftime(strf, 4095, str, tm) == 0) {
        fprintf(stderr, "strftime returned 0\n");
        exit(EXIT_FAILURE);
    }

    imlib_context_set_image(im);
    for (c = strf; *c != '\0'; c++) {
        if (*c == '$') {
            c++;
            switch (*c) {
            case 'a':
                gethostname(buf, sizeof(buf));
                strlcat(ret, buf, sizeof(ret));
                break;
            case 'f':
                if (filenameIM)
                    strcat(ret, filenameIM);
                break;
            case 'm': /* t was already taken, so m as in mini */
                if (filenameThumb)
                    strcat(ret, filenameThumb);
                break;
            case 'n':
                if (filenameIM) {
                    tmp = strrchr(filenameIM, '/');
                    if (tmp)
                        strcat(ret, tmp + 1);
                    else
                        strcat(ret, filenameIM);
                }
                break;
            case 'w':
                snprintf(buf, sizeof(buf), "%d", imlib_image_get_width());
                strcat(ret, buf);
                break;
            case 'h':
                snprintf(buf, sizeof(buf), "%d", imlib_image_get_height());
                strcat(ret, buf);
                break;
            case 's':
                if (filenameIM) {
                    if (!stat(filenameIM, &st)) {
                        int size;

                        size = st.st_size;
                        snprintf(buf, sizeof(buf), "%d", size);
                        strcat(ret, buf);
                    } else
                        strcat(ret, "[err]");
                }
                break;
            case 'p':
                snprintf(buf, sizeof(buf), "%d",
                    imlib_image_get_width() * imlib_image_get_height());
                strcat(ret, buf);
                break;
            case 't':
                tmp = imlib_image_format();
                if (tmp)
                    strcat(ret, tmp);
                break;
            case '$':
                strcat(ret, "$");
                break;
            default:
                strncat(ret, c, 1);
                break;
            }
        } else if (*c == '\\') {
            c++;
            switch (*c) {
            case 'n':
                if (filenameIM)
                    strcat(ret, "\n");
                break;
            default:
                strncat(ret, c, 1);
                break;
            }
        } else {
            const size_t length = strlen(ret);
            ret[length] = *c;
            ret[length + 1] = '\0';
        }
    }
    return strdup(ret);
}

Window scrotGetClientWindow(Display* display, Window target)
{
    Atom state;
    Atom type = None;
    int format, status;
    unsigned char* data;
    unsigned long after, items;
    Window client;

    state = XInternAtom(display, "WM_STATE", True);
    if (state == None)
        return target;
    status = XGetWindowProperty(display, target, state, 0L, 0L, False,
        AnyPropertyType, &type, &format, &items, &after,
        &data);
    if ((status == Success) && (type != None))
        return target;
    client = scrotFindWindowByProperty(display, target, state);
    if (!client)
        return target;
    return client;
}

Window scrotFindWindowByProperty(Display* display, const Window window, const Atom property)
{
    Atom type = None;
    int format, status;
    unsigned char* data;
    unsigned int i, numberChildren;
    unsigned long after, numberItems;
    Window child = None, *children, parent, root;

    status = XQueryTree(display, window, &root, &parent, &children, &numberChildren);
    if (!status)
        return None;
    for (i = 0; (i < numberChildren) && (child == None); i++) {
        status = XGetWindowProperty(display, children[i], property, 0L, 0L, False,
            AnyPropertyType, &type, &format,
            &numberItems, &after, &data);
        if (data)
            XFree(data);
        if ((status == Success) && type)
            child = children[i];
    }
    for (i = 0; (i < numberChildren) && (child == None); i++)
        child = scrotFindWindowByProperty(display, children[i], property);
    if (children != None)
        XFree(children);
    return (child);
}

Imlib_Image scrotGrabStackWindows(void)
{
    if (XGetSelectionOwner(disp, XInternAtom(disp, "_NET_WM_CM_S0", False)) == None) {
        fprintf(stderr, "Composite Manager is not running, required to use this option.\n");
        exit(EXIT_FAILURE);
    }

    unsigned long numberItemsReturn;
    unsigned long bytesAfterReturn;
    unsigned char* propReturn;
    long offset = 0L;
    long length = ~0L;
    Bool delete = False;
    int actualFormatReturn;
    Atom actualTypeReturn;
    ScrotImlibList* listImages = NULL;
    Imlib_Image im = NULL;
    XImage* ximage = NULL;
    XWindowAttributes attr;
    unsigned long i = 0;

#define EWMH_CLIENT_LIST "_NET_CLIENT_LIST" // spec EWMH

    Atom atomProp = XInternAtom(disp, EWMH_CLIENT_LIST, False);
    Atom atomType = AnyPropertyType;

    int result = XGetWindowProperty(disp, root, atomProp, offset, length,
        delete, atomType, &actualTypeReturn, &actualFormatReturn,
        &numberItemsReturn, &bytesAfterReturn, &propReturn);

    if (result != Success || numberItemsReturn == 0) {
        fprintf(stderr, "Failed XGetWindowProperty: " EWMH_CLIENT_LIST "\n");
        exit(EXIT_FAILURE);
    }

    XCompositeRedirectSubwindows(disp, root, CompositeRedirectAutomatic);

    for (i = 0; i < numberItemsReturn; i++) {
        Window win = *((Window*)propReturn + i);

        if (!XGetWindowAttributes(disp, win, &attr)) {
            fprintf(stderr, "Failed XGetWindowAttributes\n");
            exit(EXIT_FAILURE);
        }

        /* Only visible windows */
        if (attr.map_state != IsViewable)
            continue;

        if (!scrotMatchWindowClassName(win))
            continue;

        ximage = XGetImage(disp, win, 0, 0, attr.width, attr.height, AllPlanes, ZPixmap);

        if (!ximage) {
            fprintf(stderr, "Failed XGetImage: Window id 0x%lx.\n", win);
            exit(EXIT_FAILURE);
        }

        im = imlib_create_image_from_ximage(ximage, NULL, attr.x, attr.y, attr.width, attr.height, 1);

        XFree(ximage);

        listImages = appendToScrotImlib(listImages, im);
    }

    return stalkImageConcat(listImages);
}

Imlib_Image scrotGrabShotMulti(void)
{
    int screens;
    int i;
    char* dispStr;
    char* subDisp;
    char newDisp[255];
    ScrotImlibList* images = NULL;
    Imlib_Image ret = NULL;

    screens = ScreenCount(disp);
    if (screens < 2)
        return scrotGrabShot();

    subDisp = strdup(DisplayString(disp));

    for (i = 0; i < screens; i++) {
        dispStr = strchr(subDisp, ':');
        if (dispStr) {
            dispStr = strchr(dispStr, '.');
            if (dispStr)
                *dispStr = '\0';
        }
        snprintf(newDisp, sizeof(newDisp), "%s.%d", subDisp, i);
        initXAndImlib(newDisp, i);
        ret = imlib_create_image_from_drawable(0, 0, 0, scr->width,
            scr->height, 1);
        images = appendToScrotImlib(images, ret);
    }
    free(subDisp);

    ret = stalkImageConcat(images);

    return ret;
}

Imlib_Image stalkImageConcat(ScrotImlibList* images)
{
    int totalWidth = 0, maxHeight = 0, w, h;
    int x = 0;
    ScrotImlibList* l;
    ScrotImlibList* item;
    Imlib_Image ret, im;

    if (isScrotImlibListEmpty(images))
        return NULL;

    l = images;
    while (l) {
        im = l->data;
        imlib_context_set_image(im);
        h = imlib_image_get_height();
        w = imlib_image_get_width();
        if (h > maxHeight)
            maxHeight = h;
        totalWidth += w;
        l = l->next;
    }
    ret = imlib_create_image(totalWidth, maxHeight);
    imlib_context_set_image(ret);
    imlib_context_set_color(0, 0, 0, 255);
    imlib_image_fill_rectangle(0, 0, totalWidth, maxHeight);
    l = images;
    while (l) {
        im = l->data;
        item = l;
        l = l->next;
        imlib_context_set_image(im);
        h = imlib_image_get_height();
        w = imlib_image_get_width();
        imlib_context_set_image(ret);
        imlib_context_set_anti_alias(0);
        imlib_context_set_dither(1);
        imlib_context_set_blend(0);
        imlib_context_set_angle(0);
        imlib_blend_image_onto_image(im, 0, 0, 0, w, h, x, 0, w, h);
        x += w;
        imlib_context_set_image(im);
        imlib_free_image_and_decache();
        free(item);
    }
    return ret;
}
