/* scrot.h

Copyright 1999-2000 Tom Gilbert <tom@linuxbrit.co.uk,
                                  gilbertt@linuxbrit.co.uk,
                                  scrot_sucks@linuxbrit.co.uk>
Copyright 2009      James Cameron <quozl@us.netrek.org>
Copyright 2019-2020 Daniel T. Borelli <daltomi@disroot.org>
Copyright 2020      Jeroen Roovers <jer@gentoo.org>
Copyright 2020      Hinigatsu <hinigatsu@protonmail.com>
Copyright 2021      Christopher R. Nelson <christopher.nelson@languidnights.com>
Copyright 2021      Guilherme Janczak <guilherme.janczak@yandex.com>

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

#ifndef SCROT_H
#define SCROT_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <X11/keysym.h>
#include <X11/Xresource.h>
#include <X11/cursorfont.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/shape.h>

#include <Imlib2.h>
#include <ctype.h>
#include <dirent.h>
#include <err.h>
#include <errno.h>
#include <getopt.h>
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

#include "scrot_config.h"
#include "structs.h"
#include "note.h"
#include "scrot_selection.h"
#include "slist.h"

typedef void (*sighandler_t) (int);

void show_usage(void);
void show_version(void);
void show_mini_usage(void);
void init_x_and_imlib(char *dispstr, int screen_num);
char *chop_file_from_full_path(char *str);
Imlib_Image scrot_grab_shot(void);
void scrot_exec_app(Imlib_Image image, struct tm *tm,
                    char *filename_im, char *filename_thumb);
void scrot_do_delay(void);
Imlib_Image scrot_sel_and_grab_image(void);
Imlib_Image scrot_grab_focused(void);
Imlib_Image scrot_grab_autoselect(void);
void scrot_sel_area(int *x, int *y, int *w, int *h);
void scrot_nice_clip(int *rx, int *ry, int *rw, int *rh);
int scrot_get_geometry(Window target, int *rx, int *ry, int *rw, int *rh);
int scrot_match_window_class_name(Window target);
Window scrot_get_window(Display *display,Window window,int x,int y);
Window scrot_get_client_window(Display * display, Window target);
Window scrot_find_window_by_property(Display * display, const Window window,
                                     const Atom property);
char *im_printf(char *str, struct tm *tm,
                char *filename_im, char *filename_thumb,
                Imlib_Image im);
Imlib_Image scrot_grab_shot_multi(void);
Imlib_Image scrot_grab_stack_windows(void);
Imlib_Image stalk_image_concat(Scrot_Imlib_List *images);

void scrot_grab_mouse_pointer(const Imlib_Image image,
		const int ix_off, const int iy_off);

void scrot_check_if_overwrite_file(char **filename);
size_t scrot_have_file_extension(char const *filename, char **ext);

/* Imlib stuff */
extern Display *disp;
extern Visual *vis;
extern Colormap cm;
extern int depth;

/* Thumbnail sizes */
extern Window root;
extern Screen *scr;

#endif
