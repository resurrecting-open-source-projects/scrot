/* scrot.c

Copyright 1999-2000 Tom Gilbert <tom@linuxbrit.co.uk,
                                  gilbertt@linuxbrit.co.uk,
                                  scrot_sucks@linuxbrit.co.uk>
Copyright 2009      James Cameron <quozl@us.netrek.org>
Copyright 2010      Ibragimov Rinat <ibragimovrinat@mail.ru>
Copyright 2017      Stoney Sauce <stoneysauce@gmail.com>
Copyright 2019-2023 Daniel T. Borelli <danieltborelli@gmail.com>
Copyright 2019      Jade Auer <jade@trashwitch.dev>
Copyright 2020      blockparole
Copyright 2020      Cungsten Tarbide <ctarbide@tuta.io>
Copyright 2020      Hinigatsu <hinigatsu@protonmail.com>
Copyright 2020      ideal <idealities@gmail.com>
Copyright 2020      nothub
Copyright 2020      Sean Brennan <zettix1@gmail.com>
Copyright 2021      c0dev0id <sh+github@codevoid.de>
Copyright 2021      Christopher R. Nelson <christopher.nelson@languidnights.com>
Copyright 2021-2023 Guilherme Janczak <guilherme.janczak@yandex.com>
Copyright 2021      IFo Hancroft <contact@ifohancroft.com>
Copyright 2021      Peter Wu <peterwu@hotmail.com>
Copyright 2022-2023 NRK <nrk@disroot.org>
Copyright 2022      Zev Weiss <zev@bewilderbeest.net>

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

#include <sys/stat.h>
#include <sys/wait.h>

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <Imlib2.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xinerama.h>

#include "note.h"
#include "options.h"
#include "scrot.h"
#include "util.h"

static void initXAndImlib(const char *, int);
static void uninitXAndImlib(void);
static void scrotSaveImage(const char *);
static Imlib_Image scrotGrabFocused(void);
static void applyFilterIfRequired(void);
static Imlib_Image scrotGrabAutoselect(void);
static long miliToNanoSec(int);
static Imlib_Image scrotGrabShotMulti(void);
static Imlib_Image scrotGrabShotMonitor(void);
static Imlib_Image scrotGrabStackWindows(void);
static Imlib_Image scrotGrabShot(void);
static void scrotCheckIfOverwriteFile(char **);
static void scrotExecApp(Imlib_Image, struct tm *, char *, char *);
static char *imPrintf(const char *, struct tm *, const char *, const char *,
    Imlib_Image);
static char *scrotGetWindowName(Window);
static Window scrotGetClientWindow(Display *, Window);
static Window scrotFindWindowByProperty(Display *, const Window, const Atom);
static Imlib_Image stalkImageConcat(Imlib_Image *, size_t, const enum Direction);
static int findWindowManagerFrame(Window *const, int *const);
static Imlib_Image scrotGrabWindowById(Window const window);
static void scrotGrabMousePointer(Imlib_Image, const int, const int);

/* X11 stuff */
Display *disp;
Window root;
Window clientWindow;
Screen *scr;

int main(int argc, char *argv[])
{
    Imlib_Image image = NULL;
    Imlib_Image thumbnail;
    char *filenameIM = NULL;
    char *filenameThumb = NULL;
    struct timespec timeStamp;
    struct tm *tm;

    /* Get the time ASAP to reduce the timing error in case --delay is used. */
    opt.delayStart = clockNow();

    atexit(uninitXAndImlib);

    optionsParse(argc, argv);

    initXAndImlib(opt.display, 0);

    if (opt.mode == MODE_SELECT)
        image = scrotSelectionSelectMode();
    else {

        scrotDoDelay();

        if (opt.mode == MODE_FOCUSED)
            image = scrotGrabFocused();
        else if (opt.mode == MODE_MULTIDISP)
            image = scrotGrabShotMulti();
        else if (opt.mode == MODE_STACK)
            image = scrotGrabStackWindows();
        else if (opt.mode == MODE_MONITOR)
            image = scrotGrabShotMonitor();
        else if (opt.mode == MODE_AUTOSEL)
            image = scrotGrabAutoselect();
        else if (opt.mode == MODE_WINDOW)
            image = scrotGrabWindowById(opt.windowId);
        else if (opt.mode == MODE_SCREEN)
            image = scrotGrabShot();
        else
            scrotAssert(!"unreachable");
    }
    if (!image)
        errx(EXIT_FAILURE, "no image grabbed");

    /* Get the time right after the screenshot.
     * Don't put any new code between this clock_gettime() call and the
     * screenshot-taking above or it will skew the timing.
     */
    clock_gettime(CLOCK_REALTIME, &timeStamp);
    if (timeStamp.tv_nsec >= miliToNanoSec(500)) {
        /* Round the timestamp to the nearest second. */
        timeStamp.tv_sec++;
    }
    tm = localtime(&timeStamp.tv_sec);

    if (opt.note)
        scrotNoteDraw(image);

    imlib_context_set_image(image);
    imlib_image_set_format(opt.format);
    imlib_image_attach_data_value("quality", NULL, opt.quality, NULL);
    imlib_image_attach_data_value("compression", NULL, opt.compression, NULL);

    filenameIM = imPrintf(opt.outputFile, tm, NULL, NULL, image);
    scrotCheckIfOverwriteFile(&filenameIM);

    applyFilterIfRequired();

    scrotSaveImage(filenameIM);

    if (opt.thumb != THUMB_DISABLED) {
        int cwidth, cheight;
        int twidth, theight;

        cwidth = imlib_image_get_width();
        cheight = imlib_image_get_height();

        if (opt.thumb == THUMB_RES) {
            twidth = opt.thumbW;
            theight = opt.thumbH;
            if (twidth == 0)
                twidth = cwidth * opt.thumbH / cheight;
            else if (theight == 0)
                theight = cheight * opt.thumbW / cwidth;
        } else {
            twidth = cwidth * opt.thumbPercent / 100;
            theight = cheight * opt.thumbPercent / 100;
        }
        /* twidth and theight could be rounded to 0 for extremely small sizes,
         * fix them up.
         */
        if (twidth == 0)
            twidth = 1;
        if (theight == 0)
            theight = 1;

        imlib_context_set_anti_alias(1);
        thumbnail = imlib_create_cropped_scaled_image(0, 0, cwidth, cheight,
            twidth, theight);
        if (!thumbnail) {
            errx(EXIT_FAILURE, "unable to create thumbnail");
        } else {
            imlib_context_set_image(thumbnail);
            imlib_image_set_format(opt.format);

            filenameThumb = imPrintf(opt.thumbFile, tm, NULL, NULL, thumbnail);
            scrotCheckIfOverwriteFile(&filenameThumb);
            scrotSaveImage(filenameThumb);
            imlib_free_image_and_decache();
        }
    }
    if (opt.exec)
        scrotExecApp(image, tm, filenameIM, filenameThumb);

    imlib_context_set_image(image);
    imlib_free_image_and_decache();
    free(filenameIM);
    free(filenameThumb);

    return 0;
}

static void initXAndImlib(const char *dispStr, int screenNumber)
{
    disp = XOpenDisplay(dispStr);
    if (!disp) {

        const char *const format = "Can't open X display. It *is* running, "
            "yeah? [%s]";

        const char *env = NULL;

        const char *const value = dispStr ? dispStr :
            (env = getenv("DISPLAY")) ? env : "NULL";

        errx(EXIT_FAILURE, format, value);
    }

    if (screenNumber)
        scr = ScreenOfDisplay(disp, screenNumber);
    else
        scr = ScreenOfDisplay(disp, DefaultScreen(disp));

    Visual *vis = DefaultVisual(disp, XScreenNumberOfScreen(scr));
    Colormap cm = DefaultColormap(disp, XScreenNumberOfScreen(scr));
    root = RootWindow(disp, XScreenNumberOfScreen(scr));

    imlib_context_set_drawable(root);
    imlib_context_set_display(disp);
    imlib_context_set_visual(vis);
    imlib_context_set_colormap(cm);
    imlib_context_set_color_modifier(NULL);
    imlib_context_set_operation(IMLIB_OP_COPY);
    imlib_set_cache_size(0);
}

/* atexit register func. */
static void uninitXAndImlib(void)
{
    if (opt.note) {
        scrotNoteFree();
        free(opt.note);
    }

    if (disp) {
        XCloseDisplay(disp);
        disp = NULL;
    }
}

static void scrotSaveImage(const char *filename)
{
    Imlib_Load_Error imErr;
    imlib_save_image_with_error_return(filename, &imErr);
    if (imErr) {
        const char *colon = "", *errmsg = "", imlibPrefix[] = "Imlib2: ";
#if defined(IMLIB2_VERSION)
#if IMLIB2_VERSION >= IMLIB2_VERSION_(1, 10, 0)
        colon = ": ";
        errmsg = imlib_strerror(imlib_get_error());
#endif
#endif
        if (strncmp(errmsg, imlibPrefix, sizeof(imlibPrefix) - 1) == 0)
            errmsg += sizeof(imlibPrefix) - 1;
        errx(EXIT_FAILURE, "failed to save image: %s%s%s",
            filename, colon, errmsg);
    }
}

size_t scrotHaveFileExtension(const char *filename, char **ext)
{
    const char *basename = strrchr(filename, '/');
    if (!basename)
        basename = filename;
    basename += *basename == '/';
    basename += *basename == '.'; /* dot files are not extensions */
    char *s = strrchr(basename, '.');
    *ext = (s && s[1] != '\0') ? s : "";
    return strlen(*ext);
}

Imlib_Image scrotGrabRectAndPointer(int x, int y, int w, int h)
{
    Imlib_Image im = imlib_create_image_from_drawable(0, x, y, w, h, 1);
    if (!im)
        errx(EXIT_FAILURE, "failed to grab image");
    if (opt.pointer)
        scrotGrabMousePointer(im, x, y);
    return im;
}

static Imlib_Image scrotGrabWindowById(Window const window)
{
    Imlib_Image im = NULL;
    int rx = 0, ry = 0, rw = 0, rh = 0;

    if (!scrotGetGeometry(window, &rx, &ry, &rw, &rh))
        return NULL;
    scrotNiceClip(&rx, &ry, &rw, &rh);
    im = scrotGrabRectAndPointer(rx, ry, rw, rh);
    clientWindow = window;
    return im;
}

static Imlib_Image scrotGrabFocused(void)
{
    Window target = None;

    XGetInputFocus(disp, &target, &(int){0});
    return scrotGrabWindowById(target);
}

static Imlib_Image scrotGrabAutoselect(void)
{
    int rx = opt.autoselectX, ry = opt.autoselectY, rw = opt.autoselectW,
        rh = opt.autoselectH;
    scrotNiceClip(&rx, &ry, &rw, &rh);
    return scrotGrabRectAndPointer(rx, ry, rw, rh);
}

void scrotDoDelay(void)
{
    if (!opt.delay)
        return;
    if (opt.countdown) {
        fputs("Taking shot in ", stderr);
        for (int i = opt.delay; i > 0; i--) {
            /* Illumos doesn't have dprintf():
             * https://www.illumos.org/issues/1609
             */
            fprintf(stderr, "%d.. ", i);
            fflush(stderr);
            opt.delayStart = scrotSleepFor(opt.delayStart, 1000);
        }
        fputs("0.\n", stderr);
        fflush(stderr);
    } else {
        scrotSleepFor(opt.delayStart, opt.delay * 1000);
    }
}

static long miliToNanoSec(int ms)
{
    return ms * 1000L * 1000L;
}

/* clockNow() has the exact same semantics as CLOCK_MONOTONIC. Except that on
 * Linux, CLOCK_MONOTONIC does not progress while the system is suspended, so
 * the non-standard CLOCK_BOOTTIME is used instead to avoid this bug.
 */
struct timespec clockNow(void)
{
    struct timespec ret;
#if defined(__linux__)
    clock_gettime(CLOCK_BOOTTIME, &ret);
#else
    clock_gettime(CLOCK_MONOTONIC, &ret);
#endif
    return ret;
}

/* OpenBSD and OS X lack clock_nanosleep(), so we call nanosleep() and use a
 * trivial algorithm to correct for drift. The end timespec is returned for
 * callers that want it. EINTR is also dealt with.
 */
struct timespec scrotSleepFor(struct timespec start, int ms)
{
    scrotAssert(ms >= 0);
    struct timespec end = {
        .tv_sec  = start.tv_sec  + (ms / 1000),
        .tv_nsec = start.tv_nsec + miliToNanoSec(ms % 1000),
    };
    if (end.tv_nsec >= miliToNanoSec(1000)) {
        ++end.tv_sec;
        end.tv_nsec -= miliToNanoSec(1000);
    }

    struct timespec tmp;
    do {
        tmp = clockNow();

        /* XXX: Use timespecsub(). OS X doesn't have that BSD macro, and libbsd
         * doesn't support OS X save for an unmaintained fork. libobsd supports
         * OS X but doesn't have the macro yet.
         */
        tmp.tv_sec  = end.tv_sec  - tmp.tv_sec;
        tmp.tv_nsec = end.tv_nsec - tmp.tv_nsec;
        if (tmp.tv_nsec < 0) {
            --tmp.tv_sec;
            tmp.tv_nsec += miliToNanoSec(1000);
        }
    } while (nanosleep(&tmp, NULL) < 0 && errno == EINTR);

    return end;
}

/* Clip rectangle nicely */
void scrotNiceClip(int *rx, int *ry, int *rw, int *rh)
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

static int findWindowManagerFrame(Window *const target, int *const frames)
{
    int x, status;
    unsigned int d;
    Window rt, *children, parent;

    status = XGetGeometry(disp, *target, &root, &x, &x, &d, &d, &d, &d);

    if (!status)
        return 0;

    for (;;) {
        status = XQueryTree(disp, *target, &rt, &parent, &children, &d);
        if (status && (children != None))
            XFree(children);
        if (!status || (parent == None) || (parent == rt))
            break;
        *target = parent;
        ++*frames;
    }
    return 1;
}

/* Get geometry of window and use that */
int scrotGetGeometry(Window target, int *rx, int *ry, int *rw, int *rh)
{
    Window child;
    XWindowAttributes attr;
    int stat, frames = 0;

    /* Get windowmanager frame of window */
    if (target != root) {
        if (findWindowManagerFrame(&target, &frames)) {
            /* Get client window. */
            if (!opt.border)
                target = scrotGetClientWindow(disp, target);

            XRaiseWindow(disp, target);
            XSync(disp, False);

            /* HACK: there doesn't seem to be any way to figure out whether the
             * raise request was accepted or rejected. so just sleep a bit to
             * give the WM some time to update. */
            scrotSleepFor(clockNow(), 160);
        }
    }
    stat = XGetWindowAttributes(disp, target, &attr);
    if (!stat || (attr.map_state != IsViewable))
        return 0;
    *rw = attr.width;
    *rh = attr.height;
    XTranslateCoordinates(disp, target, root, 0, 0, rx, ry, &child);

    /* Special case when the TWM emulates the border directly on the window. */
    if (opt.border && frames < 2 && attr.border_width > 0) {
        *rw += attr.border_width * 2;
        *rh += attr.border_width * 2;
        *rx -= attr.border_width;
        *ry -= attr.border_width;
    }
    return 1;
}

Window scrotGetWindow(Display *display, Window window, int x, int y)
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

static void scrotGrabMousePointer(Imlib_Image image, const int xOffset,
    const int yOffset)
{
    XFixesCursorImage *xcim = XFixesGetCursorImage(disp);
    if (!xcim)
        errx(EXIT_FAILURE, "Can't get the cursor from X");
    const unsigned short width = xcim->width;
    const unsigned short height = xcim->height;
    const size_t pixcnt = (size_t)width*height;

    uint32_t *pixels = (uint32_t *)xcim->pixels;
    /* XFixesCursorImage returns pixels as `unsigned long`, which is typically
     * 64bits, but imlib2 expects 32bit packed integers. */
    if (sizeof *xcim->pixels > sizeof *pixels) {
        for (size_t i = 0; i < pixcnt; ++i)
            pixels[i] = xcim->pixels[i];
    }

    Imlib_Image imcursor = imlib_create_image_using_data(width, height, pixels);
    if (!imcursor)
        errx(EXIT_FAILURE, "Can't create cursor image");

    /* Overlay the cursor into `image`. */
    const int x = (xcim->x - xcim->xhot) - xOffset;
    const int y = (xcim->y - xcim->yhot) - yOffset;
    imlib_context_set_image(imcursor);
    imlib_image_set_has_alpha(1);
    imlib_context_set_image(image);
    imlib_blend_image_onto_image(imcursor, 0, 0, 0, width, height, x, y, width,
        height);
    imlib_context_set_image(imcursor);
    imlib_free_image();
    XFree(xcim);
}

// It assumes that the local variable 'scrot.c:Imlib_Image image' is in context
static void applyFilterIfRequired(void)
{
    if (opt.script) {
        warnx("--script is deprecated. See: "
            "https://github.com/resurrecting-open-source-projects/scrot/pull/231");
        imlib_apply_filter(opt.script);
    }
}

static void scrotCheckIfOverwriteFile(char **filename)
{
    if (opt.overwrite)
        return;

    if (access(*filename, F_OK) == -1)
        return;

    const size_t maxCounter = 999;
    char fmt[5]; // _000 + NUL byte
    const size_t slen = strlen(*filename);
    const size_t nalloc = slen + sizeof(fmt);

    char *ext;
    size_t extLength = scrotHaveFileExtension(*filename, &ext);

    char *newName = ecalloc(nalloc, sizeof(*newName));
    memcpy(newName, *filename, slen - extLength);
    char *ptr = newName + (slen - extLength);

    size_t counter = 0;
    do {
        snprintf(fmt, sizeof(fmt), "_%03zu", counter++);
        memcpy(ptr, fmt, sizeof(fmt));
        memcpy(ptr + sizeof(fmt) - 1, ext, extLength);
    } while ((counter < maxCounter) && !access(newName, F_OK));

    scrotAssert(newName[nalloc - 1] == '\0');

    if (counter == maxCounter) {
        errx(EXIT_FAILURE, "scrot can no longer generate new file names.\n"
            "The last attempt is %s", newName);
    }

    warnx("`%s` already exists, attempting `%s` instead", *filename, newName);
    free(*filename);
    *filename = newName;
}

static int scrotMatchWindowClassName(Window target)
{
    scrotAssert(disp != NULL);
    if (!opt.windowClassName)
        return 1; /* Match any if the user hasn't requested a specific class. */

    XClassHint clsHint;
    int match = 0;

    if (XGetClassHint(disp, target, &clsHint) != BadWindow) {
        match = strcmp(clsHint.res_class, opt.windowClassName) == 0;
        XFree(clsHint.res_name);
        XFree(clsHint.res_class);
    }

    return match;
}

static Imlib_Image scrotGrabShot(void)
{
    if (!opt.silent)
        XBell(disp, 0);
    return scrotGrabRectAndPointer(0, 0, scr->width, scr->height);
}

static void scrotExecApp(Imlib_Image image, struct tm *tm, char *filenameIM,
    char *filenameThumb)
{
    char *execStr = imPrintf(opt.exec, tm, filenameIM, filenameThumb, image);
    int ret = system(execStr);
    if (ret == -1)
        err(EXIT_FAILURE, "The child process could not be created");
    else if (!WIFEXITED(ret) || WEXITSTATUS(ret) != 0)
        exit(69 /* EX_UNAVAILABLE */ );
    free(execStr);
}

static char *imPrintf(const char *str, struct tm *tm, const char *filenameIM,
    const char *filenameThumb, Imlib_Image im)
{
    char buf[20];
    Stream ret = {0};
    long hostNameMax = 0;
    char *tmp;
    struct stat st;

    /* prepend a useless '.' to distinguish between valid 0 return vs 0 return
     * due to small buffer size.
     */
    Stream fmt = {0};
    streamChar(&fmt, '.');
    streamStr(&fmt, str);
    streamChar(&fmt, '\0');

    size_t strfLen, strfSize = 32;
    char *strf = NULL;
    do {
        strfSize *= 2;
        strf = erealloc(strf, strfSize);
    } while ((strfLen = strftime(strf, strfSize, fmt.buf, tm)) == 0);
    free(fmt.buf);
    if (strfLen == 0)
        errx(EXIT_FAILURE, "strftime returned 0");

    imlib_context_set_image(im);
    for (const char *c = strf + 1, *end = strf + strfLen; c < end; ++c) {
        if (*c == '$' && (c + 1) < end) {
            c++;
            switch (*c) {
            case 'a':
                /* freebsd and macos don't have HOST_NAME_MAX defined.
                 * instead sysconf is recommended by freebsd. */
                if (hostNameMax == 0)
                    hostNameMax = sysconf(_SC_HOST_NAME_MAX) + 1; /* +1 for nul-terminator */

                streamReserve(&ret, hostNameMax);
                char *target = ret.buf + ret.off;
                gethostname(target, hostNameMax);
                ret.off += strlen(target);
                scrotAssert(ret.buf[ret.off] == '\0');
                break;
            case 'F':
                streamStr(&ret, opt.format);
                break;
            case 'f':
                if (filenameIM)
                    streamStr(&ret, filenameIM);
                break;
            case 'm': /* t was already taken, so m as in mini */
                if (filenameThumb)
                    streamStr(&ret, filenameThumb);
                break;
            case 'n':
                if (filenameIM) {
                    tmp = strrchr(filenameIM, '/');
                    if (tmp)
                        streamStr(&ret, tmp + 1);
                    else
                        streamStr(&ret, filenameIM);
                }
                break;
            case 'w':
                snprintf(buf, sizeof(buf), "%d", imlib_image_get_width());
                streamStr(&ret, buf);
                break;
            case 'h':
                snprintf(buf, sizeof(buf), "%d", imlib_image_get_height());
                streamStr(&ret, buf);
                break;
            case 's':
                if (filenameIM) {
                    if (!stat(filenameIM, &st)) {
                        snprintf(buf, sizeof(buf), "%jd", (intmax_t)st.st_size);
                        streamStr(&ret, buf);
                    } else
                        streamStr(&ret, "[err]");
                }
                break;
            case 'p':
                snprintf(buf, sizeof(buf), "%d",
                    imlib_image_get_width() * imlib_image_get_height());
                streamStr(&ret, buf);
                break;
            case 't':
                tmp = imlib_image_format();
                if (tmp)
                    streamStr(&ret, tmp);
                break;
            case '$':
                streamChar(&ret, '$');
                break;
            case 'W':
                if (clientWindow && (tmp = scrotGetWindowName(clientWindow))) {
                    streamStr(&ret, tmp);
                    XFree(tmp);
                }
                break;
            default:
                streamChar(&ret, *c);
                break;
            }
        } else if (*c == '\\' && (c + 1) < end) {
            c++;
            switch (*c) {
            case 'n':
                if (filenameIM)
                    streamChar(&ret, '\n');
                break;
            default:
                streamChar(&ret, *c);
                break;
            }
        } else {
            streamChar(&ret, *c);
        }
    }
    free(strf);
    streamChar(&ret, '\0');
    return ret.buf;
}

/* return value should be freed by XFree() */
static char *scrotGetWindowName(Window window)
{
    scrotAssert(disp != NULL);
    scrotAssert(window != None);

    if (window == root)
        return NULL;

    if (!findWindowManagerFrame(&window, &(int){0}))
        return NULL;

    XClassHint clsHint;
    char *windowName = NULL;

    const Status status = XGetClassHint(disp,
            scrotGetClientWindow(disp, window),
            &clsHint);

    if (status != 0) {
        windowName = clsHint.res_class;
        XFree(clsHint.res_name);
    }
    return windowName;
}

static Window scrotGetClientWindow(Display *display, Window target)
{
    Atom state;
    Atom type = None;
    int format, status;
    unsigned char *data = NULL;
    unsigned long after, items;
    Window client;

    state = XInternAtom(display, "WM_STATE", True);
    if (state == None)
        return target;
    status = XGetWindowProperty(display, target, state, 0L, 0L, False,
        AnyPropertyType, &type, &format, &items, &after,
        &data);
    XFree(data);
    if ((status == Success) && (type != None))
        return target;
    client = scrotFindWindowByProperty(display, target, state);
    if (!client)
        return target;
    return client;
}

static Window scrotFindWindowByProperty(Display *display, const Window window,
    const Atom property)
{
    Atom type = None;
    int format, status;
    unsigned char *data = NULL;
    unsigned int i, numberChildren;
    unsigned long after, numberItems;
    Window child = None, *children, parent, rootReturn;

    status = XQueryTree(display, window, &rootReturn, &parent, &children,
        &numberChildren);
    if (!status)
        return None;
    for (i = 0; (i < numberChildren) && (child == None); i++) {
        status = XGetWindowProperty(display, children[i], property, 0L, 0L, False,
            AnyPropertyType, &type, &format,
            &numberItems, &after, &data);
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

static Imlib_Image scrotGrabStackWindows(void)
{
    if (XGetSelectionOwner(disp, XInternAtom(disp, "_NET_WM_CM_S0", False))
        == None) {
        warnx("option --stack: Composite Manager is not running,"
            " some windows may overlap.");
    }

    unsigned long numberItemsReturn;
    unsigned long bytesAfterReturn;
    unsigned char *propReturn;
    long offset = 0L;
    long length = ~0L;
    Bool delete = False;
    int actualFormatReturn;
    Atom actualTypeReturn;
    Imlib_Image im = NULL;
    XImage *ximage = NULL;
    XWindowAttributes attr;
    unsigned long i = 0;
    char EWMH_CLIENT_LIST[] = "_NET_CLIENT_LIST"; // spec EWMH

    Atom atomProp = XInternAtom(disp, EWMH_CLIENT_LIST, False);
    Atom atomType = AnyPropertyType;

    int result = XGetWindowProperty(disp, root, atomProp, offset, length,
        delete, atomType, &actualTypeReturn, &actualFormatReturn,
        &numberItemsReturn, &bytesAfterReturn, &propReturn);

    if (result != Success || numberItemsReturn == 0) {
        errx(EXIT_FAILURE, "option --stack: Failed XGetWindowProperty: %s",
            EWMH_CLIENT_LIST);
    }

    size_t imagesCount = 0;
    Imlib_Image *images = erealloc(NULL, numberItemsReturn * sizeof(*images));

    if (!XCompositeQueryVersion(disp, &(int){0}, &(int){0}))
        errx(EXIT_FAILURE, "XCompositeQueryVersion() failed");

    XCompositeRedirectSubwindows(disp, root, CompositeRedirectAutomatic);

    for (i = 0; i < numberItemsReturn; i++) {
        Window win = *((Window *)propReturn + i);

        if (!XGetWindowAttributes(disp, win, &attr))
            errx(EXIT_FAILURE, "option --stack: Failed XGetWindowAttributes");

        /* Only visible windows */
        if (attr.map_state != IsViewable)
            continue;

        if (!scrotMatchWindowClassName(win))
            continue;

        ximage = XGetImage(disp, win, 0, 0, attr.width, attr.height, AllPlanes,
            ZPixmap);

        if (!ximage) {
            errx(EXIT_FAILURE,
                "option --stack: Failed XGetImage: Window id 0x%lx", win);
        }

        im = imlib_create_image_from_ximage(ximage, NULL, 0, 0,
            attr.width, attr.height, 1);
        if (!im) {
            errx(EXIT_FAILURE, "option --stack: "
                "Failed to create Imlib2 image: Window id 0x%lx", win);
        }

        XDestroyImage(ximage);

        images[imagesCount++] = im;
    }
    XFree(propReturn);

    return stalkImageConcat(images, imagesCount, opt.stackDirection);
}

static Imlib_Image scrotGrabShotMulti(void)
{
    int screens = ScreenCount(disp);
    if (screens < 2)
        return scrotGrabShot();

    int i;
    char *dispStr;
    char *subDisp;
    char newDisp[255];
    Imlib_Image ret = NULL;
    size_t imagesCount = 0;
    Imlib_Image *images = erealloc(NULL, screens * sizeof(*images));

    subDisp = estrdup(DisplayString(disp));

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
        if (!ret)
            errx(EXIT_FAILURE, "failed to grab image");

        images[imagesCount++] = ret;
    }
    free(subDisp);

    return stalkImageConcat(images, imagesCount, HORIZONTAL);
}

static Imlib_Image scrotGrabShotMonitor(void)
{
    int eventBase, errBase;

    if (!XineramaQueryExtension(disp, &eventBase, &errBase))
        errx(EXIT_FAILURE, "Xinerama extension not found");

    int numScreens = 0;
    XineramaScreenInfo *screens = XineramaQueryScreens(disp, &numScreens);

    if (!screens && !numScreens)
        errx(EXIT_FAILURE, "Xinerama not active");

    if (!numScreens)
        errx(EXIT_FAILURE, "Xinerama active but did not find any output device");

    if (opt.monitor >= numScreens)
        errx(EXIT_FAILURE, "monitor %d not found", opt.monitor);

    scrotAssert(screens); /* silence clang-tidy */
    XineramaScreenInfo *mon = &screens[opt.monitor];
    int x = mon->x_org, y = mon->y_org, w = mon->width, h = mon->height;
    XFree(screens);

    scrotNiceClip(&x, &y, &w, &h);
    return scrotGrabRectAndPointer(x, y, w, h);
}

static Imlib_Image stalkImageConcat(
    Imlib_Image *images, size_t imagesCount, const enum Direction dir)
{
    if (imagesCount == 0)
        return NULL;

    int total = 0, max = 0;
    int x = 0, y = 0, w , h;
    Imlib_Image ret, im;

    const bool vertical = (dir == VERTICAL) ? true : false;

    for (size_t i = 0; i < imagesCount; ++i) {
        im = images[i];
        imlib_context_set_image(im);
        h = imlib_image_get_height();
        w = imlib_image_get_width();
        if (!vertical) {
            if (h > max)
                max = h;
            total += w;
        } else {
            if (w > max)
                max = w;
            total += h;
        }
    }
    if (!vertical) {
        w = total;
        h = max;
    } else {
        w = max;
        h = total;
    }
    ret = imlib_create_image(w, h);

    imlib_context_set_image(ret);
    imlib_context_set_color(0, 0, 0, 255);
    imlib_image_fill_rectangle(0, 0, w, h);

    imlib_context_set_anti_alias(0);
    imlib_context_set_dither(1);
    imlib_context_set_blend(0);

    for (size_t i = 0; i < imagesCount; ++i) {
        im = images[i];
        imlib_context_set_image(im);
        h = imlib_image_get_height();
        w = imlib_image_get_width();
        imlib_context_set_image(ret);
        imlib_blend_image_onto_image(im, 0, 0, 0, w, h, x, y, w, h);
        (!vertical) ? (x += w) : (y += h);
        imlib_context_set_image(im);
        imlib_free_image_and_decache();
    }
    free(images);
    return ret;
}
