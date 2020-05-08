/* options.h

Copyright 1999-2000 Tom Gilbert <tom@linuxbrit.co.uk,
                                  gilbertt@linuxbrit.co.uk,
                                  scrot_sucks@linuxbrit.co.uk>
Copyright 2009      James Cameron <quozl@us.netrek.org>
Copyright 2010      Ibragimov Rinat <ibragimovrinat@mail.ru>
Copyright 2017      Stoney Sauce <stoneysauce@gmail.com>
Copyright 2019      Daniel T. Borelli <danieltborelli@gmail.com>
Copýright 2020      Sean Brennan <zettix1@gmail.com>

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

#ifndef OPTIONS_H
#define OPTIONS_H

struct __scrotoptions
{
   int debug_level;
   int delay;
   int countdown;
   int select;
   int focused;
   int quality;
   int border;
   int silent;   
   int multidisp;
   int thumb;
   int thumb_width;
   int thumb_height;
   int pointer;
   int freeze;
   int overwrite;
   int line_style;
   int line_width;
   int stack;
   char *line_color;
   char *output_file;
   char *thumb_file;
   char *exec;
   char *display;
   char *note;
   int autoselect;
   int autoselect_x;
   int autoselect_y;
   int autoselect_h;
   int autoselect_w;
};

void init_parse_options(int argc, char **argv);
char *name_thumbnail(char *name);
void options_parse_thumbnail(char *optarg);
void options_parse_autoselect(char *optarg);
void options_parse_display(char *optarg);
void options_parse_note(char *optarg);
int  options_parse_required_number(char *str);
extern scrotoptions opt;

#endif
