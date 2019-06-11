/* main.c

Copyright 1999-2000 Tom Gilbert <tom@linuxbrit.co.uk,
                                  gilbertt@linuxbrit.co.uk,
                                  scrot_sucks@linuxbrit.co.uk>
Copyright 2009      James Cameron <quozl@us.netrek.org>
Copyright 2010      Ibragimov Rinat <ibragimovrinat@mail.ru>
Copyright 2017      Stoney Sauce <stoneysauce@gmail.com>
Copyright 2019      Daniel T. Borelli <danieltborelli@gmail.com>

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

  init_x_and_imlib(NULL, 0);

  if (!opt.output_file) {
    opt.output_file = gib_estrdup("%Y-%m-%d-%H%M%S_$wx$h_scrot.png");
    opt.thumb_file = gib_estrdup("%Y-%m-%d-%H%M%S_$wx$h_scrot-thumb.png");
  } else {
    have_extension = scrot_have_file_extension(opt.output_file);
  }


  if (opt.focused)
    image = scrot_grab_focused();
  else if (opt.select)
    image = scrot_sel_and_grab_image();
  else if (opt.autoselect)
    image = gib_imlib_create_image_from_drawable(root, 0, opt.autoselect_x, opt.autoselect_y, opt.autoselect_w, opt.autoselect_h, 1);
  else
  {
    scrot_do_delay();
    if (opt.multidisp) {
      image = scrot_grab_shot_multi();
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

char* scrot_have_file_extension(char *filename)
{
    char *ext = strrchr(filename, '.');
    return (ext && (strlen(ext + 1) == 3)) ? ext : NULL;
}

void scrot_check_if_overwrite_file(char **filename)
{
  char *curfile = *filename;

  if (opt.overwrite == 1) return;

  if (access(curfile, F_OK) == -1) return;

  const int max_count = 999;
  static int count = 0;
  const char *extension = scrot_have_file_extension(curfile);
  const size_t slen = strlen(curfile);
  int nalloc = slen + 4 + 1; // _000 + NUL byte
  char fmt[5];
  char *newname = NULL;

  if (extension)
    nalloc += 4; // .ext

  newname = calloc(nalloc, sizeof(char));

  if (extension)
    // exclude extension
    memcpy(newname, curfile, slen - 4);
  else
    memcpy(newname, curfile, slen);

  do {
    snprintf(fmt, 5, "_%03d", count++);

    if (!extension) {
      strncpy(newname + slen, fmt, 5);
    } else {
        strncpy((newname + slen)-4, fmt, 5);
        strncat(newname, extension, 4);
    }
      curfile = newname;
  } while ((count < max_count) && (access(curfile, F_OK) == 0));

  free(*filename);
  *filename = newname;

  if (count == max_count) {
    fprintf(stderr, "scrot can no longer generate new file names.\n"
                    "The last attempt is %s\n", newname);
    free(newname);
    exit(EXIT_FAILURE);
  }
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
  pixels = xcim->pixels;
#else
  DATA32 data[width * height * 4];

  for (size_t i = 0; i < (width * height); i++)
    ((DATA32*)data)[i] = (DATA32)xcim->pixels[i];

  pixels = data;
#endif

  Imlib_Image imcursor  = imlib_create_image_using_data(width, height, pixels);

  XFree(xcim);

  if (!imcursor) {
     fprintf(stderr, "scrot_grab_mouse_pointer: Failed create image using data.");
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

  if (opt.note != NULL)
    scrot_note_draw(im);

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
    fprintf(stderr, "scrot could not be executed the command: %s.\n", execstr);
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
  int rect_x = 0, rect_y = 0, rect_w = 0, rect_h = 0;
  Cursor cursor, cursor2;
  Window target = None;
  GC gc;
  XGCValues gcval;

  xfd = ConnectionNumber(disp);
  fdsize = xfd + 1;

  cursor = XCreateFontCursor(disp, XC_left_ptr);
  cursor2 = XCreateFontCursor(disp, XC_lr_angle);

  gcval.foreground = XWhitePixel(disp, 0);
  gcval.function = GXxor;
  gcval.background = XBlackPixel(disp, 0);
  gcval.plane_mask = gcval.background ^ gcval.foreground;
  gcval.subwindow_mode = IncludeInferiors;

  gc =
    XCreateGC(disp, root,
              GCFunction | GCForeground | GCBackground | GCSubwindowMode,
              &gcval);

  if ((XGrabPointer
       (disp, root, False,
        ButtonMotionMask | ButtonPressMask | ButtonReleaseMask, GrabModeAsync,
        GrabModeAsync, root, cursor, CurrentTime) != GrabSuccess)) {
    fprintf(stderr, "couldn't grab pointer:");
    XFreeCursor(disp, cursor);
    XFreeCursor(disp, cursor2);
    XFreeGC(disp, gc);
    exit(EXIT_FAILURE);
  }


  if ((XGrabKeyboard
       (disp, root, False, GrabModeAsync, GrabModeAsync,
        CurrentTime) != GrabSuccess)) {
    fprintf(stderr, "couldn't grab keyboard:");
    XFreeCursor(disp, cursor);
    XFreeCursor(disp, cursor2);
    XFreeGC(disp, gc);
    exit(EXIT_FAILURE);
  }

  if (opt.freeze == 1)
      XGrabServer(disp);

  while (1) {
    /* handle events here */
    while (!done && XPending(disp)) {
      XNextEvent(disp, &ev);
      switch (ev.type) {
        case MotionNotify:
          if (btn_pressed) {
            if (rect_w) {
              /* re-draw the last rect to clear it */
              XDrawRectangle(disp, root, gc, rect_x, rect_y, rect_w, rect_h);
            } else {
              /* Change the cursor to show we're selecting a region */
              XChangeActivePointerGrab(disp,
                                       ButtonMotionMask | ButtonReleaseMask,
                                       cursor2, CurrentTime);
            }

            rect_x = rx;
            rect_y = ry;
            rect_w = ev.xmotion.x - rect_x;
            rect_h = ev.xmotion.y - rect_y;

            if (rect_w < 0) {
              rect_x += rect_w;
              rect_w = 0 - rect_w;
            }
            if (rect_h < 0) {
              rect_y += rect_h;
              rect_h = 0 - rect_h;
            }
            /* draw rectangle */
            XDrawRectangle(disp, root, gc, rect_x, rect_y, rect_w, rect_h);
            XFlush(disp);
          }
          break;
        case ButtonPress:
          btn_pressed = 1;
          rx = ev.xbutton.x;
          ry = ev.xbutton.y;
          target =
            scrot_get_window(disp, ev.xbutton.subwindow, ev.xbutton.x,
                             ev.xbutton.y);
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
      fprintf(stderr, "Connection to X display lost");
      exit(EXIT_FAILURE);
    }
  }
  if (rect_w) {
    XDrawRectangle(disp, root, gc, rect_x, rect_y, rect_w, rect_h);
    XFlush(disp);
  }
  XUngrabPointer(disp, CurrentTime);
  XUngrabKeyboard(disp, CurrentTime);
  XFreeCursor(disp, cursor);
  XFreeCursor(disp, cursor2);
  XFreeGC(disp, gc);
  XSync(disp, True);

  if (opt.freeze == 1)
      XUngrabServer(disp);

  if (done < 2) {
    scrot_do_delay();
    if (rect_w > 5) {
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
  int stat;

  /* get windowmanager frame of window */
  if (target != root) {
    unsigned int d;
    int x;
    int status;
    
    status = XGetGeometry(disp, target, &root, &x, &x, &d, &d, &d, &d);
    if (status != 0) {
      Window rt, *children, parent;
      
      for (;;) {
	/* Find window manager frame. */
	status = XQueryTree(disp, target, &rt, &parent, &children, &d);
	if (status && (children != None))
	  XFree((char *) children);
	if (!status || (parent == None) || (parent == rt))
	  break;
	target = parent;
      }
      /* Get client window. */
      if (!opt.border)
	target = scrot_get_client_window(disp, target);
      XRaiseWindow(disp, target);
    }
  }
  stat = XGetWindowAttributes(disp, target, &attr);
  if ((stat == False) || (attr.map_state != IsViewable))
    return 0;
  *rw = attr.width;
  *rh = attr.height;
  XTranslateCoordinates(disp, target, root, 0, 0, rx, ry, &child);
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
  strftime(strf, 4095, str, tm);

  for (c = strf; *c != '\0'; c++) {
    if (*c == '$') {
      c++;
      switch (*c) {
        case 'f':
          if (filename_im)
            strcat(ret, filename_im);
          break;
        case 'm': /* t was allready taken, so m as in mini */
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
    } else
      strncat(ret, c, 1);
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

  dispstr = DisplayString(disp);

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
