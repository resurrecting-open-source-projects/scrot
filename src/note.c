/* note.c

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

#include <assert.h>
#include <stdint.h>

#include "scrot.h"

scrotnote note;

static Imlib_Font imfont = NULL;

static void load_font(void);


void scrot_note_new(char *format)
{
   scrot_note_free();

   note = (scrotnote){NULL, NULL, -1, -1, -1};

   char *tok = strtok(format, ":");

   while (tok) {
     const char type = tok[0];

     switch(type) {
     case 'f':
        if (tok[1] == '=') {
           if (-1 == sscanf(tok, "f=%m[^/]/%d", &note.font, &note.size)) {
              free(note.font);
              note.font = NULL;
              note.size = -1;
            }
        }
     break;
     case 'x':
        if (tok[1] == '=') {
           if (-1 == sscanf(tok, "x=%d", &note.x))
              note.x = -1;
        }
     break;
     case 'y':
        if (tok[1] == '=') {
           if (-1 == sscanf(tok, "y=%d", &note.y))
              note.y = -1;
        }
     break;
     case 't':
        if (tok[1] == '=') {
            const size_t size = strlen(tok) - 2;
            note.text = malloc(size + 1);
            assert(NULL != note.text);
            strncpy(note.text, tok + 2, size);
            note.text[size] = '\0';
         }
     break;
     default:
      fprintf(stderr, "Malformed syntax, unknown option: %c\n", tok[0]);
      scrot_note_free();
      exit(EXIT_FAILURE);
     }

     tok = strtok(NULL, ":");
   }

   int error = 0;

   if (!note.font || note.size < 0) {
      fprintf(stderr, "Malformed syntax for f=\n");
      error = 1;
   }

   if (!note.text) {
      fprintf(stderr, "Malformed syntax for t=\n");
      error = 1;
   }

   if (note.x < 0) {
      fprintf(stderr, "Malformed syntax for x=\n");
      error = 1;
   }

   if (note.y < 0) {
      fprintf(stderr, "Malformed syntax for y=\n");
      error = 1;
   }

   if (error) {
      scrot_note_free();
      exit(EXIT_FAILURE);
   }

   load_font();

}

void scrot_note_free(void)
{
    if (note.text) (free(note.text), note.text = NULL);
    if (note.font) (free(note.font), note.font = NULL);

    if (imfont) {
       imlib_context_set_font(imfont);
       imlib_free_font();
    }
}


void scrot_note_draw(Imlib_Image im)
{
   imlib_context_set_image(im);
   imlib_context_set_font(imfont);
   imlib_text_draw(note.x, note.y, note.text);
}

void load_font(void)
{
   const size_t len = strlen(note.font) + 1 + 3;
   char *fontname = malloc(len);

   snprintf(fontname, len, "%s/%d", note.font, note.size);

   imlib_add_path_to_font_path("/usr/share/fonts/TTF");

   imfont = imlib_load_font(fontname);

   if (!imfont) {
      scrot_note_free();
      fprintf(stderr, "Failed to load fontname: %s\n", fontname);
      exit(EXIT_FAILURE);
   }
}
