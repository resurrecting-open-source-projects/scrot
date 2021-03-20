/* scrot_selection_edge.c

Copyright 2020  daltomi <daltomi@disroot.org>
Copyright 2020  Daniel T. Borelli <daltomi@disroot.org>

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

extern void selection_calculate_rect(int x0, int y0, int x1, int y1);
extern struct selection_t** selection_get(void);

struct selection_edge_t {
    Window wndDraw;
    XClassHint* classHint;
};


void selection_edge_create(void)
{ 
    struct selection_t *const sel = *selection_get();

    sel->edge = calloc(1, sizeof(struct selection_edge_t));

    struct selection_edge_t *const pe = sel->edge;

    // It is ok that in the "classic" mode it is to NULL, but it is not "edge" mode
    if (opt.line_color == NULL) {
        opt.line_color = "gray";
    }

    XColor color;

    if (0 == XAllocNamedColor(disp, XDefaultColormap(disp, DefaultScreen(disp)), 
                opt.line_color, &color, &(XColor){})) 
    {
        fprintf(stderr, "Error allocate color:%s\n", strerror(BadColor));
        scrot_selection_destroy();
        exit(EXIT_FAILURE);
    }

    XSetWindowAttributes attr;
    attr.background_pixel = color.pixel;
    attr.override_redirect = True;

    pe->classHint = XAllocClassHint();
    pe->classHint->res_name  = "scrot";
    pe->classHint->res_class = "scrot";

    pe->wndDraw = XCreateWindow(disp, root, 0, 0, WidthOfScreen(scr), HeightOfScreen(scr), 0,
          CopyFromParent, InputOutput, CopyFromParent, CWOverrideRedirect | CWBackPixel, &attr);

    unsigned long opacity = opt.line_opacity * ((unsigned)-1 / 100);

    XChangeProperty(disp, pe->wndDraw, XInternAtom(disp, "_NET_WM_WINDOW_OPACITY", False),
                  XA_CARDINAL, 32, PropModeReplace,
                  (unsigned char*)&opacity, 1L);

    XChangeProperty(disp, pe->wndDraw, XInternAtom(disp, "_NET_WM_WINDOW_TYPE", False),
                  XA_ATOM, 32, PropModeReplace,
                  (unsigned char*)&(Atom){XInternAtom(disp, "_NET_WM_WINDOW_TYPE_DOCK", False)},
                  1L);

    XSetClassHint(disp, pe->wndDraw, pe->classHint);
}


void selection_edge_destroy(void)
{
    struct selection_t const *const sel = *selection_get();
    struct selection_edge_t *pe = sel->edge;

    XUnmapWindow(disp, pe->wndDraw);
    XFree(pe->classHint);
    XDestroyWindow(disp, pe->wndDraw);
    free(pe);
}


void selection_edge_draw(void)
{
    struct selection_t const *const sel = *selection_get();
    struct selection_edge_t const *const  pe = sel->edge;

    XRectangle rects[4] = {
            {sel->rect.x, sel->rect.y, opt.line_width, sel->rect.h},                                    //left
            {sel->rect.x, sel->rect.y, sel->rect.w, opt.line_width},                                    //top
            {sel->rect.x + sel->rect.w, sel->rect.y, opt.line_width, sel->rect.h},                      //right
            {sel->rect.x, sel->rect.y + sel->rect.h, sel->rect.w + opt.line_width, opt.line_width}      //bottom
    };

    XShapeCombineRectangles(disp, pe->wndDraw, ShapeBounding, 0, 0, rects, 4, ShapeSet, 0);
    XMapWindow(disp, pe->wndDraw);
}


void selection_edge_motion_draw(int x0, int y0, int x1, int y1)
{
    struct selection_t *const sel = *selection_get();

    selection_calculate_rect(x0, y0, x1, y1);

    sel->rect.x -= opt.line_width;
    sel->rect.y -= opt.line_width;
    sel->rect.w += opt.line_width;
    sel->rect.h += opt.line_width;

    selection_edge_draw();
}
