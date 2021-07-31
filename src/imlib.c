/* imlib.c

Copyright 1999-2000 Tom Gilbert
Copyright 2020      Daniel T. Borelli <daltomi@disroot.org>
Copyright 2020      ideal <idealities@gmail.com>
Copyright 2020      Sean Brennan <zettix1@gmail.com>
Copyright 2021      Christopher R. Nelson <christopher.nelson@languidnights.com>

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

#include "scrot.h"
#include "options.h"

Display *disp;
Visual *vis;
Screen *scr;
Colormap cm;
int depth;
Window root;

void init_x_and_imlib(char *dispstr, int screen_num)
{
    const char *badstr;

    disp = XOpenDisplay(dispstr);
    if (!disp) {
	    if ((badstr = dispstr) == NULL)
            if ((badstr = getenv("DISPLAY")) == NULL)
                badstr = "(null)";
        errx(EXIT_FAILURE, "can't open X display %s", badstr);
    }

    if (!screen_num)
        screen_num = DefaultScreen(disp);
    scr = ScreenOfDisplay(disp, screen_num);

    vis = DefaultVisual(disp, XScreenNumberOfScreen(scr));
    depth = DefaultDepth(disp, XScreenNumberOfScreen(scr));
    cm = DefaultColormap(disp, XScreenNumberOfScreen(scr));
    root = RootWindow(disp, XScreenNumberOfScreen(scr));

    imlib_context_set_drawable(root);
    imlib_context_set_display(disp);
    imlib_context_set_visual(vis);
    imlib_context_set_colormap(cm);
    imlib_context_set_color_modifier(NULL);
    imlib_context_set_operation(IMLIB_OP_COPY);
}
