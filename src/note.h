/* note.h

Copyright 2019  Daniel T. Borelli <danieltborelli@gmail.com>

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

/* This file is part of the scrot project. */
 
#ifndef NOTE_H
#define NOTE_H

#include "options.h"

/*
 * Format: f=STRING/NUM:x=NUM:y=NUM:t:STRING:c=NUM,NUM,NUM,NUM
 *
 * f= fontname/size
 * x= screen position x
 * y= screen position y
 * t= string note
 * c= color(red,green,blue,alpha) range 0..255
 *
 * */

struct __scrotnote
{
   char *font;    /* font name         */
   char *text;    /* text of the note  */
   int size;      /* font size         */
   int x;         /* position screen   */
   int y;         /* position screen   */

   struct color   /* optional          */
   {
      int n;      /* counter colors found
                   * -1 == error parser
                   *  0 == the user did not indicate the color
                   * */

      int r,      /* red               */
          g,      /* green             */
          b,      /* blue              */
          a;      /* alpha             */
   } color;
};

extern scrotnote note;

void scrot_note_new(char *format);

void scrot_note_free(void);

void scrot_note_draw(Imlib_Image im);

#endif
