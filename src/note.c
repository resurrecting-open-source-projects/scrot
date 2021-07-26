/* note.c

Copyright 2019-2020 Daniel T. Borelli <daltomi@disroot.org>

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

static char *parse_text(char **tok, char *const end);

static char *new_text(const char *src, const size_t len);

static inline void pfree(char **ptr)
{
   free(*ptr);
   *ptr = NULL;
}

static inline void next_space(char **tok)
{
  while (*++*tok == ' ' && **tok != '\0');
}

static inline void next_not_space(char **tok)
{
  while (*++*tok != ' ' && **tok != '\0');
}

void scrot_note_new(char *format)
{
   scrot_note_free();

   note = (scrotnote){NULL,NULL,0,0,0.0,{COLOR_OPTIONAL,0,0,0,0}};

   char *const end = format + strlen(format);

   char *tok = strpbrk(format, "-");

   if ((NULL == tok) || (strlen(tok) == 1)) {

malformed:

      pfree(&format);
      fprintf(stderr, "Error --note option : Malformed syntax.\n");
      exit(EXIT_FAILURE);
   }

   while (tok != NULL) {

     const char type = *++tok;

     next_space(&tok);

     switch(type) {
     case 'f': {
        note.font = parse_text(&tok, end);

        if (NULL == note.font) {
           fprintf(stderr, "Error --note option : Malformed syntax for -f\n");
           exit(EXIT_FAILURE);
        }

        char *number = strrchr(note.font, '/');

        if (NULL == number) {
           fprintf(stderr, "Error --note option : Malformed syntax for -f, required number.\n");
           exit(EXIT_FAILURE);
        }

        int fntsize = options_parse_required_number(++number);

        if (fntsize < 6)
           fprintf(stderr, "Warning: --note option: font size < 6\n");
     }
     break;
     case 'x': {
        if ((1 != sscanf(tok, "%d", &note.x) || (note.x < 0))) {
           fprintf(stderr, "Error --note option : Malformed syntax for -x\n");
           exit(EXIT_FAILURE);
        }
        next_not_space(&tok);
     }
     break;
     case 'y': {
        if ((1 != sscanf(tok, "%d", &note.y)) || (note.y < 0)) {
           fprintf(stderr, "Error --note option : Malformed syntax for -y\n");
           exit(EXIT_FAILURE);
        }
        next_not_space(&tok);
     }
     break;
     case 'a': {
        if ((1 != sscanf(tok, "%lf", &note.angle))) {
           fprintf(stderr, "Error --note option : Malformed syntax for -a\n");
           exit(EXIT_FAILURE);
        }
        next_not_space(&tok);
     }
     break;
     case 't':
        note.text = parse_text(&tok, end);
        if (NULL == note.text) {
           fprintf(stderr, "Error --note option : Malformed syntax for -t\n");
           exit(EXIT_FAILURE);
        }
     break;
     case 'c': {
        char *saveptr = NULL;

        char *c = strtok_r(tok, ",", &saveptr);

        int nclrs = 0;

        note.color.status = COLOR_OK;

        while (c) {

           tok = c;

           int color = options_parse_required_number(c);

           if ((color < 0) || color > 255) {
              fprintf(stderr, "Error --note option : color '%d' out of range 0..255\n", color);
              note.color.status = COLOR_ERROR;
              break;
           }

           switch(++nclrs) {
           case 1 : note.color.r = color; break;
           case 2 : note.color.g = color; break;
           case 3 : note.color.b = color; break;
           case 4 : note.color.a = color; break;
           }

           c = strtok_r(NULL, ",", &saveptr);
        }

        if (nclrs != 4) {
           fprintf(stderr, "Error --note option : Malformed syntax for -c\n");
           note.color.status = COLOR_ERROR;
        }
     }
     break;
     default:
        fprintf(stderr, "Error --note option : unknown option: '-%c'\n", type);
        exit(EXIT_FAILURE);
     }

     tok = strpbrk(tok, "-");
   }

   if ((NULL == note.font) || (NULL == note.text))
     goto malformed;

   load_font();
}

void scrot_note_free(void)
{
    if (note.text) pfree(&note.text);
    if (note.font) pfree(&note.font);

    if (imfont) {
       imlib_context_set_font(imfont);
       imlib_free_font();
    }
}

void scrot_note_draw(Imlib_Image im)
{
   if (NULL == im) return;

   imlib_context_set_image(im);
   imlib_context_set_font(imfont);

   imlib_context_set_direction(IMLIB_TEXT_TO_ANGLE);
   imlib_context_set_angle(note.angle);

   if (note.color.status == COLOR_OK)
      imlib_context_set_color(note.color.r,
                              note.color.g,
                              note.color.b,
                              note.color.a);

   imlib_text_draw(note.x, note.y, note.text);
}

void load_font(void)
{
   imfont = imlib_load_font(note.font);

   if (!imfont) {
      fprintf(stderr, "Error --note option : Failed to load fontname: %s\n", note.font);
      scrot_note_free();
      exit(EXIT_FAILURE);
   }
}

char *parse_text(char **tok, char *const end)
{
   assert(NULL != *tok);
   assert(NULL != end);

   if (**tok != '\'') return NULL;

   (*tok)++;

   char *begin = *tok;

   while ((*tok != end) && **tok != '\'') {
      (*tok)++;
   }

   ptrdiff_t len = (*tok - begin);

   if (len == 0) return NULL;

   return new_text(begin, len);
}

char *new_text(const char *src, const size_t len)
{
   assert(NULL != src);

   char *text = malloc(len + 1);

   assert(NULL != text);

   strncpy(text, src, len);

   text[len] = '\0';

   return text;
}
