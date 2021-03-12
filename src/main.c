/* main.c

Copyright 1999-2000 Tom Gilbert <tom@linuxbrit.co.uk,
                                  gilbertt@linuxbrit.co.uk,
                                  scrot_sucks@linuxbrit.co.uk>
Copyright 2009      James Cameron <quozl@us.netrek.org>
Copyright 2010      Ibragimov Rinat <ibragimovrinat@mail.ru>
Copyright 2017      Stoney Sauce <stoneysauce@gmail.com>
Copyright 2019      Daniel T. Borelli <danieltborelli@gmail.com>
Copyright 2019      Jade Auer <jade@trashwitch.dev>
Copyright 2020      blockparole
Copyright 2020      Cungsten Tarbide <ctarbide@tuta.io>
Copyright 2020      daltomi <daltomi@disroot.org>
Copyright 2020      Hinigatsu <hinigatsu@protonmail.com>
Copyright 2020      nothub
Copyright 2020      Sean Brennan <zettix1@gmail.com>
Copyright 2020      spycapitan <spycapitan@protonmail.com>
Copyright 2021      c0dev0id <sh+github@codevoid.de>

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
#include <assert.h>


/* atexit register func. */
static void uninit_x_and_imlib(void)
{
  if (disp) {
     XCloseDisplay(disp);
     disp = NULL;
  }
}


int
main(int argc,
     char **argv)
{
  Imlib_Image image;
  Imlib_Image thumbnail;
  Imlib_Load_Error err;
  char *filename_im = NULL, *filename_thumb = NULL;

  char *have_extension = NULL;

  time_t t;
  struct tm *tm;

  init_parse_options(argc, argv);

  init_x_and_imlib(opt.display, 0);

  atexit(uninit_x_and_imlib);

  if (!opt.output_file) {
    opt.output_file = gib_estrdup("%Y-%m-%d-%H%M%S_$wx$h_scrot.png");
    opt.thumb_file = gib_estrdup("%Y-%m-%d-%H%M%S_$wx$h_scrot-thumb.png");
  } else {
    scrot_have_file_extension(opt.output_file, &have_extension);
  }


  if (opt.focused)
    image = scrot_grab_focused();
  else if (opt.select)
    image = scrot_sel_and_grab_image();
  else if (opt.autoselect)
    image = scrot_grab_autoselect();
  else
  {
    scrot_do_delay();
    if (opt.multidisp) {
      image = scrot_grab_shot_multi();
    } else if (opt.stack) {
      image = scrot_grab_stack_windows();
    } else {
      image = scrot_grab_shot();
    }
  }

  if (opt.note != NULL)
    scrot_note_draw(image);

  if (!image)
    gib_eprintf("no image grabbed");

  time(&t); /* Get the time directly after the screenshot */
  tm = localtime(&t);

  imlib_context_set_image(image);
  imlib_image_attach_data_value("quality", NULL, opt.quality, NULL);

  if (!have_extension) {
    imlib_image_set_format("png");
  }

  filename_im = im_printf(opt.output_file, tm, NULL, NULL, image);
  scrot_check_if_overwrite_file(&filename_im);

  gib_imlib_save_image_with_error_return(image, filename_im, &err);
  if (err)
    gib_eprintf("Saving to file %s failed\n", filename_im);
  if (opt.thumb)
  {
    int cwidth, cheight;
    int twidth, theight;

    cwidth = gib_imlib_image_get_width(image);
    cheight = gib_imlib_image_get_height(image);

    /* Geometry based thumb size */
    if (opt.thumb_width || opt.thumb_height)
    {
      if (!opt.thumb_width)
      {
        twidth = cwidth * opt.thumb_height / cheight;
        theight = opt.thumb_height;
      }
      else if (!opt.thumb_height)
      {
        twidth = opt.thumb_width;
        theight = cheight * opt.thumb_width / cwidth;
      }
      else
      {
        twidth = opt.thumb_width;
        theight = opt.thumb_height;
      }
    }
    else
    {
      twidth = cwidth * opt.thumb / 100;
      theight = cheight * opt.thumb / 100;
    }

    thumbnail =
      gib_imlib_create_cropped_scaled_image(image, 0, 0, cwidth, cheight,
                                            twidth, theight, 1);
    if (thumbnail == NULL)
      gib_eprintf("Unable to create scaled Image\n");
    else
    {
      if (opt.note != NULL)
        scrot_note_draw(image);

      scrot_have_file_extension(opt.thumb_file, &have_extension);

      if (!have_extension) {
        imlib_context_set_image(thumbnail);
        imlib_image_set_format("png");
      }

      filename_thumb = im_printf(opt.thumb_file, tm, NULL, NULL, thumbnail);
      scrot_check_if_overwrite_file(&filename_thumb);
      gib_imlib_save_image_with_error_return(thumbnail, filename_thumb, &err);
      if (err)
        gib_eprintf("Saving thumbnail %s failed\n", filename_thumb);
    }
  }
  if (opt.exec)
    scrot_exec_app(image, tm, filename_im, filename_thumb);
  gib_imlib_free_image_and_decache(image);

  return 0;
}

void
scrot_do_delay(void)
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

size_t scrot_have_file_extension(char const *filename, char **ext)
{
    *ext = strrchr(filename, '.');

    if (*ext) {
        return strlen(*ext);
    }

    return 0;
}


void scrot_check_if_overwrite_file(char **filename)
{
  char *curfile = *filename;

  if (opt.overwrite == 1) return;

  if (access(curfile, F_OK) == -1) return;

  const size_t max_counter = 999;
  size_t counter = 0;
  char *ext = NULL;
  size_t ext_len = 0;
  const size_t slen = strlen(curfile);
  size_t nalloc = slen + 4 + 1; // _000 + NUL byte
  char fmt[5];
  char *newname = NULL;

  ext_len = scrot_have_file_extension(curfile, &ext);

  if (ext)
    nalloc += ext_len; // .ext

  newname = calloc(nalloc, sizeof(char));

  if (ext)
    // exclude ext
    memcpy(newname, curfile, slen - ext_len);
  else
    memcpy(newname, curfile, slen);

  do {
    snprintf(fmt, 5, "_%03zu", counter++);

    if (!ext) {
      strncpy(newname + slen, fmt, 5);
    } else {
        strncpy((newname + slen) - ext_len, fmt, 5);
        strncat(newname, ext, ext_len);
    }
      curfile = newname;
  } while ((counter < max_counter) && (access(curfile, F_OK) == 0));

  free(*filename);
  *filename = newname;

  if (counter == max_counter) {
    fprintf(stderr, "scrot can no longer generate new file names.\n"
                    "The last attempt is %s\n", newname);
    free(newname);
    exit(EXIT_FAILURE);
  }
}


int scrot_match_window_class_name(Window target)
{
    assert(disp != NULL);

    const int NOT_MATCH = 0;
    const int MATCH = 1;
    /* By default all class names match since window_class_name by default is NULL*/
    int retval = MATCH;

    if (opt.window_class_name == NULL) {
        return retval;
    }

    XClassHint classHint;
    retval = NOT_MATCH; // window_class_name != NULL, by default NOT_MATCH

    if (XGetClassHint(disp, target, &classHint) != BadWindow) {
        retval = options_cmp_window_class_name(classHint.res_class);
        XFree(classHint.res_name);
        XFree(classHint.res_class);
    }

    return retval;
}


void
scrot_grab_mouse_pointer(const Imlib_Image image,
    const int ix_off, const int iy_off)
{
  XFixesCursorImage *xcim = XFixesGetCursorImage(disp);

  const int width       = xcim->width;
  const int height      = xcim->height;
  const int x           = (xcim->x - xcim->xhot) - ix_off;
  const int y           = (xcim->y - xcim->yhot) - iy_off;
  DATA32 *pixels        = NULL;

#ifdef __i386__
  pixels = (DATA32*)xcim->pixels;
#else
  DATA32 data[width * height * 4];
  
  size_t i;
  for (i = 0; i < (width * height); i++)
    ((DATA32*)data)[i] = (DATA32)xcim->pixels[i];

  pixels = data;
#endif

  Imlib_Image imcursor  = imlib_create_image_using_data(width, height, pixels);

  XFree(xcim);

  if (!imcursor) {
     fprintf(stderr, "scrot_grab_mouse_pointer: Failed create image using data.\n");
     exit(EXIT_FAILURE);
  }

  imlib_context_set_image(imcursor);
  imlib_image_set_has_alpha(1);
  imlib_context_set_image(image);
  imlib_blend_image_onto_image(imcursor, 0, 0, 0, width, height, x, y, width, height);
  imlib_context_set_image(imcursor);
  imlib_free_image();
}


Imlib_Image
scrot_grab_shot(void)
{
  Imlib_Image im;

  if (! opt.silent) XBell(disp, 0);

  im =
    gib_imlib_create_image_from_drawable(root, 0, 0, 0, scr->width,
                                         scr->height, 1);
  if (opt.pointer == 1)
    scrot_grab_mouse_pointer(im, 0, 0);

  return im;
}

void
scrot_exec_app(Imlib_Image image, struct tm *tm,
               char *filename_im, char *filename_thumb)
{
  char *execstr;
  int ret;

  execstr = im_printf(opt.exec, tm, filename_im, filename_thumb, image);

  errno = 0;

  ret = system(execstr);

  if (ret == -1) {
    fprintf(stderr, "The child process could not be created: %s\n", strerror(errno));
  } else if (WEXITSTATUS(ret) == 127) {
    fprintf(stderr, "scrot could not execute the command: %s.\n", execstr);
  }

  exit(0);
}

Imlib_Image
scrot_grab_focused(void)
{
  Imlib_Image im = NULL;
  int rx = 0, ry = 0, rw = 0, rh = 0;
  Window target = None;
  int ignored;

  scrot_do_delay();
  XGetInputFocus(disp, &target, &ignored);
  if (!scrot_get_geometry(target, &rx, &ry, &rw, &rh)) return NULL;
  scrot_nice_clip(&rx, &ry, &rw, &rh);
  im = gib_imlib_create_image_from_drawable(root, 0, rx, ry, rw, rh, 1);
  if (opt.pointer == 1)
	  scrot_grab_mouse_pointer(im, rx, ry);
  return im;
}

Imlib_Image
scrot_sel_and_grab_image(void)
{
  Imlib_Image im = NULL;
  static int xfd = 0;
  static int fdsize = 0;
  XEvent ev;
  fd_set fdset;
  int count = 0, done = 0;
  int rx = 0, ry = 0, rw = 0, rh = 0, btn_pressed = 0;
  Window target = None;
  Status ret;

  scrot_selection_create();

  xfd = ConnectionNumber(disp);
  fdsize = xfd + 1;

  ret = XGrabKeyboard(disp, root, False, GrabModeAsync, GrabModeAsync, CurrentTime);
  if (ret == AlreadyGrabbed) {
    int attempts = 20;
    struct timespec delay = {0, 50 * 1000L * 1000L};
    do {
      nanosleep(&delay, NULL);
      ret = XGrabKeyboard(disp, root, False, GrabModeAsync, GrabModeAsync, CurrentTime);
    } while (--attempts > 0 && ret == AlreadyGrabbed);
  }
  if (ret != GrabSuccess) {
    fprintf(stderr, "failed to grab keyboard\n");
    scrot_selection_destroy();
    exit(EXIT_FAILURE);
  }


  while (1) {
    /* handle events here */
    while (!done && XPending(disp)) {
      XNextEvent(disp, &ev);
      switch (ev.type) {
        case MotionNotify:
          if (btn_pressed) {
              scrot_selection_motion_draw(rx, ry, ev.xbutton.x, ev.xbutton.y);
          }
          break;
        case ButtonPress:
          btn_pressed = 1;
          rx = ev.xbutton.x;
          ry = ev.xbutton.y;
          target = scrot_get_window(disp, ev.xbutton.subwindow, ev.xbutton.x, ev.xbutton.y);
          if (target == None)
            target = root;
          break;
        case ButtonRelease:
          done = 1;
          break;
        case KeyPress:
          fprintf(stderr, "Key was pressed, aborting shot\n");
          done = 2;
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
    FD_ZERO(&fdset);
    FD_SET(xfd, &fdset);
    errno = 0;
    count = select(fdsize, &fdset, NULL, NULL, NULL);
    if ((count < 0)
        && ((errno == ENOMEM) || (errno == EINVAL) || (errno == EBADF))) {
      fprintf(stderr, "Connection to X display lost\n");
      scrot_selection_destroy();
      exit(EXIT_FAILURE);
    }
  }
  scrot_selection_draw();

  XUngrabKeyboard(disp, CurrentTime);

  bool const isAreaSelect = (bool)(scrot_selection_get_rect()->w > 5);

  scrot_selection_destroy();

  if (done < 2) {
    scrot_do_delay();
    if (isAreaSelect) {
      /* if a rect has been drawn, it's an area selection */
      rw = ev.xbutton.x - rx;
      rh = ev.xbutton.y - ry;

      if (rw < 0) {
        rx += rw;
        rw = 0 - rw;
      }
      if (rh < 0) {
        ry += rh;
        rh = 0 - rh;
      }

      // Not record pointer if there is a selection area because it is busy on that
      opt.pointer = 0;

    } else {
      /* else it's a window click */
      if (!scrot_get_geometry(target, &rx, &ry, &rw, &rh)) return NULL;
    }
    scrot_nice_clip(&rx, &ry, &rw, &rh);

    if (! opt.silent) XBell(disp, 0);
    im = gib_imlib_create_image_from_drawable(root, 0, rx, ry, rw, rh, 1);

    if (opt.pointer == 1)
       scrot_grab_mouse_pointer(im, rx, ry);
  }
  return im;
}

Imlib_Image
scrot_grab_autoselect(void)
{
  Imlib_Image im = NULL;
  int rx = opt.autoselect_x, ry = opt.autoselect_y, rw = opt.autoselect_w, rh = opt.autoselect_h;

  scrot_do_delay();
  scrot_nice_clip(&rx, &ry, &rw, &rh);
  im = gib_imlib_create_image_from_drawable(root, 0, rx, ry, rw, rh, 1);
  if (opt.pointer == 1)
	  scrot_grab_mouse_pointer(im, rx, ry);
  return im;
}

/* clip rectangle nicely */
void
scrot_nice_clip(int *rx,
  int *ry,
  int *rw,
  int *rh)
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

static Bool scrot_xevent_visibility(Display *dpy, XEvent *ev, XPointer arg)
{
    Window *win = (Window*)arg;
    return (ev->xvisibility.window == *win);
}

/* get geometry of window and use that */
int
scrot_get_geometry(Window target,
		   int *rx,
		   int *ry,
		   int *rw,
		   int *rh)
{
  Window child;
  XWindowAttributes attr;
  int stat, frames = 0;

  /* get windowmanager frame of window */
  if (target != root) {
    unsigned int d;
    int x;
    int status;

    status = XGetGeometry(disp, target, &root, &x, &x, &d, &d, &d, &d);
    if (status != 0) {
      Window rt, *children, parent;
      XEvent ev;

      for (;;) {
        /* Find window manager frame. */
        status = XQueryTree(disp, target, &rt, &parent, &children, &d);
        if (status && (children != None)) {
          XFree((char *) children);
        }
        if (!status || (parent == None) || (parent == rt)) {
            break;
        }
        target = parent;
        ++frames;
      }
      /* Get client window. */
      if (!opt.border) {
        target = scrot_get_client_window(disp, target);
      }
      XRaiseWindow(disp, target);

      /* Give the WM time to update the hidden area of the window.
         Some windows never send the event, a time limit is placed.
      */
      XSelectInput(disp, target, FocusChangeMask);
      for(short i = 0; i < 30; ++i) {
        if (XCheckIfEvent(disp, &ev, &scrot_xevent_visibility, (XPointer)&target) == True) {
            break;
        }
        usleep(2000);
      }
    }
  }
  stat = XGetWindowAttributes(disp, target, &attr);
  if ((stat == False) || (attr.map_state != IsViewable))
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

Window
scrot_get_window(Display * display,
                 Window window,
                 int x,
                 int y)
{
  Window source, target;

  int status, x_offset, y_offset;

  source = root;
  target = window;
  if (window == None)
    window = root;
  while (1) {
    status =
      XTranslateCoordinates(display, source, window, x, y, &x_offset,
                            &y_offset, &target);
    if (status != True)
      break;
    if (target == None)
      break;
    source = window;
    window = target;
    x = x_offset;
    y = y_offset;
  }
  if (target == None)
    target = window;
  return (target);
}


char *
im_printf(char *str, struct tm *tm,
          char *filename_im,
          char *filename_thumb,
          Imlib_Image im)
{
  char *c;
  char buf[20];
  char ret[4096];
  char strf[4096];
  char *tmp;
  struct stat st;

  ret[0] = '\0';

  if (strftime(strf, 4095, str, tm) == 0) {
    fprintf(stderr, "strftime returned 0\n");
    exit(EXIT_FAILURE);
  }

  for (c = strf; *c != '\0'; c++) {
    if (*c == '$') {
      c++;
      switch (*c) {
        case 'a':
          gethostname(buf, sizeof(buf));
          strcat(ret, buf);
          break;
        case 'f':
          if (filename_im)
            strcat(ret, filename_im);
          break;
        case 'm': /* t was already taken, so m as in mini */
          if (filename_thumb)
            strcat(ret, filename_thumb);
          break;
        case 'n':
          if (filename_im) {
            tmp = strrchr(filename_im, '/');
            if (tmp)
              strcat(ret, tmp + 1);
            else
              strcat(ret, filename_im);
          }
          break;
        case 'w':
          snprintf(buf, sizeof(buf), "%d", gib_imlib_image_get_width(im));
          strcat(ret, buf);
          break;
        case 'h':
          snprintf(buf, sizeof(buf), "%d", gib_imlib_image_get_height(im));
          strcat(ret, buf);
          break;
        case 's':
          if (filename_im) {
            if (!stat(filename_im, &st)) {
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
                   gib_imlib_image_get_width(im) *
                   gib_imlib_image_get_height(im));
          strcat(ret, buf);
          break;
        case 't':
          tmp = gib_imlib_image_format(im);
          if (tmp) {
            strcat(ret, gib_imlib_image_format(im));
          }
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
          if (filename_im)
            strcat(ret, "\n");
          break;
        default:
          strncat(ret, c, 1);
          break;
      }
    } else {
      const size_t len = strlen(ret);
      ret[len] = *c;
      ret[len + 1] = '\0';
    }
  }
  return gib_estrdup(ret);
}

Window
scrot_get_client_window(Display * display,
                        Window target)
{
  Atom state;
  Atom type = None;
  int format, status;
  unsigned char *data;
  unsigned long after, items;
  Window client;

  state = XInternAtom(display, "WM_STATE", True);
  if (state == None)
    return target;
  status =
    XGetWindowProperty(display, target, state, 0L, 0L, False,
                       (Atom) AnyPropertyType, &type, &format, &items, &after,
                       &data);
  if ((status == Success) && (type != None))
    return target;
  client = scrot_find_window_by_property(display, target, state);
  if (!client)
    return target;
  return client;
}

Window
scrot_find_window_by_property(Display * display,
                              const Window window,
                              const Atom property)
{
  Atom type = None;
  int format, status;
  unsigned char *data;
  unsigned int i, number_children;
  unsigned long after, number_items;
  Window child = None, *children, parent, root;

  status =
    XQueryTree(display, window, &root, &parent, &children, &number_children);
  if (!status)
    return None;
  for (i = 0; (i < number_children) && (child == None); i++) {
    status =
      XGetWindowProperty(display, children[i], property, 0L, 0L, False,
                         (Atom) AnyPropertyType, &type, &format,
                         &number_items, &after, &data);
    if (data)
      XFree(data);
    if ((status == Success) && (type != (Atom) NULL))
      child = children[i];
  }
  for (i = 0; (i < number_children) && (child == None); i++)
    child = scrot_find_window_by_property(display, children[i], property);
  if (children != None)
    XFree(children);
  return (child);
}

Imlib_Image
scrot_grab_stack_windows(void)
{
    if (XGetSelectionOwner(disp, XInternAtom(disp, "_NET_WM_CM_S0", False)) == None) {
        fprintf(stderr, "Composite Manager is not running, required to use this option.\n");
        exit(EXIT_FAILURE);
    }

    unsigned long nitems_return;
    unsigned long bytes_after_return;
    unsigned char *prop_return;
    long long_offset = 0L;
    long long_length = ~0L;
    Bool delete      = False;
    int actual_format_return;
    Atom actual_type_return;
    gib_list *list_images   = NULL;
    Imlib_Image im          = NULL;
    XImage *ximage          = NULL;
    XWindowAttributes attr;

#define EWMH_CLIENT_LIST "_NET_CLIENT_LIST" // spec EWMH

    Atom atom_prop = XInternAtom(disp, EWMH_CLIENT_LIST, False);
    Atom atom_type = AnyPropertyType;

    int result = XGetWindowProperty(disp, root, atom_prop, long_offset, long_length,
                                delete, atom_type, &actual_type_return, &actual_format_return,
                                &nitems_return, &bytes_after_return, &prop_return);

    if (result != Success || nitems_return == 0) {
       fprintf(stderr, "Failed XGetWindowProperty: " EWMH_CLIENT_LIST "\n");
       exit(EXIT_FAILURE);
    }

    XCompositeRedirectSubwindows(disp, root, CompositeRedirectAutomatic);

    int i;
    for (i = 0; i < nitems_return; i++) {

        Window win = *((Window*)prop_return + i);

        if (0 == XGetWindowAttributes(disp, win, &attr)) {
           fprintf(stderr, "Failed XGetWindowAttributes\n");
           exit(EXIT_FAILURE);
        }

        /* Only visible windows */
        if (attr.map_state != IsViewable) {
            continue;
        }

        if (!scrot_match_window_class_name(win)) {
            continue;
        }

        ximage = XGetImage(disp, win, 0, 0, attr.width, attr.height, AllPlanes, ZPixmap);

        if (ximage == NULL) {
            fprintf(stderr, "Failed XGetImage: Window id 0x%lx.\n", win);
            exit(EXIT_FAILURE);
        }

        im = imlib_create_image_from_ximage(ximage, NULL, attr.x, attr.y, attr.width, attr.height, 1);

        XFree(ximage);

        list_images = gib_list_add_end(list_images, im);
    }

    return stalk_image_concat(list_images);
}

Imlib_Image
scrot_grab_shot_multi(void)
{
  int screens;
  int i;
  char *dispstr, *subdisp;
  char newdisp[255];
  gib_list *images = NULL;
  Imlib_Image ret = NULL;

  screens = ScreenCount(disp);
  if (screens < 2)
    return scrot_grab_shot();

  subdisp = gib_estrdup(DisplayString(disp));

  for (i = 0; i < screens; i++) {
    dispstr = strchr(subdisp, ':');
    if (dispstr) {
      dispstr = strchr(dispstr, '.');
      if (NULL != dispstr)
        *dispstr = '\0';
    }
    snprintf(newdisp, sizeof(newdisp), "%s.%d", subdisp, i);
    init_x_and_imlib(newdisp, i);
    ret =
      gib_imlib_create_image_from_drawable(root, 0, 0, 0, scr->width,
                                           scr->height, 1);
    images = gib_list_add_end(images, ret);
  }
  free(subdisp);

  ret = stalk_image_concat(images);

  return ret;
}

Imlib_Image
stalk_image_concat(gib_list * images)
{
  int tot_w = 0, max_h = 0, w, h;
  int x = 0;
  gib_list *l, *item;
  Imlib_Image ret, im;

  if (gib_list_length(images) == 0)
    return NULL;

  l = images;
  while (l) {
    im = (Imlib_Image) l->data;
    h = gib_imlib_image_get_height(im);
    w = gib_imlib_image_get_width(im);
    if (h > max_h)
      max_h = h;
    tot_w += w;
    l = l->next;
  }
  ret = imlib_create_image(tot_w, max_h);
  gib_imlib_image_fill_rectangle(ret, 0, 0, tot_w, max_h, 255, 0, 0, 0);
  l = images;
  while (l) {
    im = (Imlib_Image) l->data;
    item = l;
    l = l->next;
    h = gib_imlib_image_get_height(im);
    w = gib_imlib_image_get_width(im);
    gib_imlib_blend_image_onto_image(ret, im, 0, 0, 0, w, h, x, 0, w, h, 1, 0,
                                     0);
    x += w;
    gib_imlib_free_image_and_decache(im);
    free(item);
  }
  return ret;
}
