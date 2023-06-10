/* scrot.h

Copyright 2021-2023 Guilherme Janczak <guilherme.janczak@yandex.com>
Copyright 2022      Daniel T. Borelli <danieltborelli@gmail.com>
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

/* Part of the code comes from the scrot.c file and maintains its authorship. */

#ifndef H_SCROT
#define H_SCROT

#include <time.h>

#include <X11/Xlib.h>
#include <Imlib2.h>

extern Display *disp;
extern Window root;
extern Window clientWindow;
extern Screen *scr;

Window scrotGetWindow(Display *, Window, int, int);
int scrotGetGeometry(Window, int *, int *, int *, int *);
void scrotNiceClip(int *, int *, int *, int *);
struct timespec clockNow(void);
struct timespec scrotSleepFor(struct timespec, int);
void scrotDoDelay(void);
Imlib_Image scrotGrabRectAndPointer(int x, int y, int w, int h);
size_t scrotHaveFileExtension(const char *, char **);

#endif /* !defined(H_SCROT) */
