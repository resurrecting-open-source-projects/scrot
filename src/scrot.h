/* scrot.h

Copyright 1999-2000 Tom Gilbert <tom@linuxbrit.co.uk,
                                  gilbertt@linuxbrit.co.uk,
                                  scrot_sucks@linuxbrit.co.uk>
Copyright 2009      James Cameron <quozl@us.netrek.org>
Copyright 2019-2021 Daniel T. Borelli <daltomi@disroot.org>
Copyright 2020      Jeroen Roovers <jer@gentoo.org>
Copyright 2020      Hinigatsu <hinigatsu@protonmail.com>
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

#pragma once

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/shape.h>
#include <X11/keysym.h>

#include <Imlib2.h>
#include <ctype.h>
#include <dirent.h>
#include <err.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "note.h"
#include "scrot_config.h"
#include "scrot_selection.h"
#include "slist.h"
#include "structs.h"

typedef void (*signalHandler)(int);

void showUsage(void);
void showVersion(void);
void initXAndImlib(char*, int);
char* chopFileFromFullPath(char*);
Imlib_Image scrotGrabShot(void);
void scrotExecApp(Imlib_Image, struct tm*, char*, char*);
void scrotDoDelay(void);
Imlib_Image scrotSelAndGrabImage(void);
Imlib_Image scrotGrabFocused(void);
Imlib_Image scrotGrabAutoselect(void);
void scrotSelArea(int*, int*, int*, int*);
void scrotNiceClip(int*, int*, int*, int*);
int scrotGetGeometry(Window, int*, int*, int*, int*);
int scrotMatchWindowClassName(Window);
Window scrotGetWindow(Display*, Window, int, int);
Window scrotGetClientWindow(Display*, Window);
Window scrotFindWindowByProperty(Display*, const Window, const Atom);
char* imPrintf(char*, struct tm*, char*, char*, Imlib_Image);
Imlib_Image scrotGrabShotMulti(void);
Imlib_Image scrotGrabStackWindows(void);
Imlib_Image stalkImageConcat(ScrotList*);

void scrotGrabMousePointer(const Imlib_Image, const int, const int);

void scrotCheckIfOverwriteFile(char**);
size_t scrotHaveFileExtension(char const*, char**);

/* Imlib stuff */
extern Display* disp;
extern Visual* vis;
extern Colormap cm;
extern int depth;

/* Thumbnail sizes */
extern Window root;
extern Screen* scr;
