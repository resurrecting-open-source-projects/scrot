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

   note = (scrotnote){NULL, NULL, -1, -1, {0,0}};

   char *tok = strtok(format, "-");

   while (tok) {

     const char type = tok[0];

     if (tok[1] == ' ') tok += 2;

     switch(type) {
     case 'f':
           if ((-1 == sscanf(tok, "%ms", &note.font)) ||
               (NULL == strchr(note.font, '/'))) {
                free(note.font);
                note.font = NULL;
            }
     break;
     case 'x':
           if (-1 == sscanf(tok, "%d", &note.x))
              note.x = -1;
     break;
     case 'y':
           if (-1 == sscanf(tok, "%d", &note.y))
              note.y = -1;
     break;
     case 't':
     {
            if (tok[0] != '\'') break;

            char *end = strchr(++tok, '\'');

            if ((end == NULL) || (*end == '\0')) break;

            const ptrdiff_t len = end - tok;

            note.text = malloc(len + 1);
            assert(NULL != note.text);

            strncpy(note.text, tok, len);
            note.text[len] = '\0';

            tok = tok + len + 1;
     }
     break;
     case 'c':
     {
           char *saveptr;
           char *c = strtok_r(tok, ",", &saveptr);

           while (c) {

              int color = options_parse_required_number(c);

              if ((color < 0) || color > 255) {
                 fprintf(stderr, "color '%d' out of range 0..255\n", color);
                 note.color.n = -1;
                 break;
              }

              note.color.n++;

              switch(note.color.n) {
              case 1 : note.color.r = color;
              case 2 : note.color.g = color;
              case 3 : note.color.b = color;
              case 4 : note.color.a = color;
              }

              c = strtok_r(NULL, ",", &saveptr);
           }

           if (note.color.n != 4)
              note.color.n = -1;
     }
     break;
     default:
           fprintf(stderr, "Malformed syntax, unknown option: %c\n", tok[0]);
           scrot_note_free();
           exit(EXIT_FAILURE);
     }

     tok = strtok(NULL, "-");
   }

   int error = 0;

   if (!note.font) {
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

   if (note.color.n == -1) {
      fprintf(stderr, "Malformed syntax for c=\n");
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

   if (note.color.n != 0)
      imlib_context_set_color(note.color.r,
                              note.color.g,
                              note.color.b,
                              note.color.a);

   imlib_text_draw(note.x, note.y, note.text);
}

void load_font(void)
{
   imlib_add_path_to_font_path("/usr/share/fonts/TTF");

   imfont = imlib_load_font(note.font);

   if (!imfont) {
      fprintf(stderr, "Failed to load fontname: %s\n", note.font);
      scrot_note_free();
      exit(EXIT_FAILURE);
   }
}
