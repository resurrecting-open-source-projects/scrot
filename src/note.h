/* note.h

Copyright 2019-2021 Daniel T. Borelli <danieltborelli@gmail.com>
Copyright 2021      Christopher R. Nelson <christopher.nelson@languidnights.com>
Copyright 2021      Peter Wu <peterwu@hotmail.com>
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

#pragma once

#include <Imlib2.h>

/*
 * Format: -f 'NAME/SIZE' -x NUM -y NUM -t 'TEXT' -c NUM,NUM,NUM,NUM
 *
 * -f fontname/size - absolute path
 * -x screen position x
 * -y screen position y
 * -t text note
 * -c color(red,green,blue,alpha) range 0..255
 *
 * */

struct ScrotNote {
    char *font; /* font name                   */
    char *text; /* text of the note            */
    int x; /* position screen (optional)  */
    int y; /* position screen (optional)  */
    double angle; /* angle text (optional)       */

    struct Color { /*                 (optional)  */
        int r, /* red             */
            g, /* green           */
            b, /* blue            */
            a; /* alpha           */
    } color;
};

extern struct ScrotNote note;

void scrotNoteNew(char *);
void scrotNoteFree(void);
void scrotNoteDraw(Imlib_Image);
