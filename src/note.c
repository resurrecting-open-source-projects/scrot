/* note.c

Copyright 2019-2020 Daniel T. Borelli <daltomi@disroot.org>
Copyright 2021 Guilherme Janczak <guilherme.janczak@yandex.com>

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

ScrotNote note;

static Imlib_Font imFont = NULL;

static void loadFont(void);

static char* parseText(char**, char* const);

static inline void pfree(char** ptr)
{
    free(*ptr);
    *ptr = NULL;
}

static inline void nextSpace(char** token)
{
    while (*++*token == ' ' && **token != '\0') { }
}

static inline void nextNotSpace(char** token)
{
    while (*++*token != ' ' && **token != '\0') { }
}

void scrotNoteNew(char* format)
{
    scrotNoteFree();

    note = (ScrotNote) { NULL, NULL, 0, 0, 0.0,
        { ColorOptional, 0, 0, 0, 0 } };

    char* const end = format + strlen(format);

    char* token = strpbrk(format, "-");

    if (!token || (strlen(token) == 1)) {
    malformed:

        pfree(&format);
        fprintf(stderr, "Error --note option : Malformed syntax.\n");
        exit(EXIT_FAILURE);
    }

    while (token) {
        const char type = *++token;
        char* savePtr = NULL;
        char* c;

        nextSpace(&token);

        switch (type) {
        case 'f':
            note.font = parseText(&token, end);

            if (!note.font) {
                fprintf(stderr, "Error --note option : Malformed syntax for -f\n");
                exit(EXIT_FAILURE);
            }

            char* number = strrchr(note.font, '/');

            if (!number) {
                fprintf(stderr, "Error --note option : Malformed syntax for -f, required number.\n");
                exit(EXIT_FAILURE);
            }

            int fontSize = optionsParseRequiredNumber(++number);

            if (fontSize < 6)
                fprintf(stderr, "Warning: --note option: font size < 6\n");
            break;
        case 'x':
            if ((1 != sscanf(token, "%d", &note.x) || (note.x < 0))) {
                fprintf(stderr, "Error --note option : Malformed syntax for -x\n");
                exit(EXIT_FAILURE);
            }
            nextNotSpace(&token);
            break;
        case 'y':
            if ((1 != sscanf(token, "%d", &note.y)) || (note.y < 0)) {
                fprintf(stderr, "Error --note option : Malformed syntax for -y\n");
                exit(EXIT_FAILURE);
            }
            nextNotSpace(&token);
            break;
        case 'a':
            if ((1 != sscanf(token, "%lf", &note.angle))) {
                fprintf(stderr, "Error --note option : Malformed syntax for -a\n");
                exit(EXIT_FAILURE);
            }
            nextNotSpace(&token);
            break;
        case 't':
            note.text = parseText(&token, end);
            if (!note.text) {
                fprintf(stderr, "Error --note option : Malformed syntax for -t\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'c':
            c = strtok_r(token, ",", &savePtr);

            int numberColors = 0;

            note.color.status = ColorOK;

            while (c) {
                token = c;

                int color = optionsParseRequiredNumber(c);

                if ((color < 0) || color > 255) {
                    fprintf(stderr, "Error --note option : color '%d' out of range 0..255\n", color);
                    note.color.status = ColorError;
                    break;
                }

                switch (++numberColors) {
                case 1:
                    note.color.r = color;
                    break;
                case 2:
                    note.color.g = color;
                    break;
                case 3:
                    note.color.b = color;
                    break;
                case 4:
                    note.color.a = color;
                    break;
                }

                c = strtok_r(NULL, ",", &savePtr);
            }

            if (numberColors != 4) {
                fprintf(stderr, "Error --note option : Malformed syntax for -c\n");
                note.color.status = ColorError;
            }
            break;
        default:
            fprintf(stderr, "Error --note option : unknown option: '-%c'\n", type);
            exit(EXIT_FAILURE);
        }

        token = strpbrk(token, "-");
    }

    if (!note.font || !note.text)
        goto malformed;

    loadFont();
}

void scrotNoteFree(void)
{
    if (note.text)
        pfree(&note.text);
    if (note.font)
        pfree(&note.font);

    if (imFont) {
        imlib_context_set_font(imFont);
        imlib_free_font();
    }
}

void scrotNoteDraw(Imlib_Image im)
{
    if (!im)
        return;

    imlib_context_set_image(im);
    imlib_context_set_font(imFont);

    imlib_context_set_direction(IMLIB_TEXT_TO_ANGLE);
    imlib_context_set_angle(note.angle);

    if (note.color.status == ColorOK)
        imlib_context_set_color(note.color.r,
            note.color.g,
            note.color.b,
            note.color.a);

    imlib_text_draw(note.x, note.y, note.text);
}

void loadFont(void)
{
    imFont = imlib_load_font(note.font);

    if (!imFont) {
        fprintf(stderr, "Error --note option : Failed to load fontname: %s\n", note.font);
        scrotNoteFree();
        exit(EXIT_FAILURE);
    }
}

char* parseText(char** token, char* const end)
{
    assert(NULL != *token);
    assert(NULL != end);

    if (**token != '\'')
        return NULL;

    (*token)++;

    char* begin = *token;

    while ((*token != end) && **token != '\'')
        (*token)++;

    ptrdiff_t length = (*token - begin);

    if (length == 0)
        return NULL;

    return strndup(begin, length);
}
