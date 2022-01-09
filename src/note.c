/* note.c

Copyright 2019-2021 Daniel T. Borelli <danieltborelli@gmail.com>
Copyright 2021-2022 Guilherme Janczak <guilherme.janczak@yandex.com>
Copyright 2021      IFo Hancroft <contact@ifohancroft.com>
Copyright 2021      Peter Wu <peterwu@hotmail.com>

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

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Imlib2.h>

#include "note.h"
#include "options.h"

enum { // default color
    DEF_COLOR_RED = 0,
    DEF_COLOR_GREEN = 0,
    DEF_COLOR_BLUE = 0,
    DEF_COLOR_ALPHA = 255
};

struct ScrotNote note;

static Imlib_Font imFont = NULL;
static void loadFont(void);
static char *parseText(char **, char *const);

static void pfree(char **ptr)
{
    free(*ptr);
    *ptr = NULL;
}

static void nextSpace(char **token)
{
    while (*++*token == ' ' && **token != '\0') { }
}

static void nextNotSpace(char **token)
{
    while (*++*token != ' ' && **token != '\0') { }
}

void scrotNoteNew(char *format)
{
    scrotNoteFree();

    note = (struct ScrotNote) {
        NULL, NULL, 0, 0, 0.0,
        { DEF_COLOR_RED, DEF_COLOR_GREEN,
          DEF_COLOR_BLUE, DEF_COLOR_ALPHA }
    };

    char *const end = format + strlen(format);

    char *token = strpbrk(format, "-");

    if (!token || (strlen(token) == 1)) {
        malformed:

            pfree(&format);
            errx(EXIT_FAILURE, "Error --note option : Malformed syntax.");
    }

    while (token) {
        const char type = *++token;
        char *savePtr = NULL;
        char *c;

        nextSpace(&token);

        switch (type) {
        case 'f':
            note.font = parseText(&token, end);

            if (!note.font)
                errx(EXIT_FAILURE, "Error --note option : Malformed syntax for -f");

            char *number = strrchr(note.font, '/');

            if (!number)
                errx(EXIT_FAILURE, "Error --note option : Malformed syntax for -f, required number.");

            int fontSize = optionsParseRequiredNumber(++number);

            if (fontSize < 6)
                warnx("Warning: --note option: font size < 6");
            break;
        case 'x':
            if ((1 != sscanf(token, "%d", &note.x) || (note.x < 0)))
                errx(EXIT_FAILURE, "Error --note option : Malformed syntax for -x");
            nextNotSpace(&token);
            break;
        case 'y':
            if ((1 != sscanf(token, "%d", &note.y)) || (note.y < 0))
                errx(EXIT_FAILURE, "Error --note option : Malformed syntax for -y");
            nextNotSpace(&token);
            break;
        case 'a':
            if ((1 != sscanf(token, "%lf", &note.angle)))
                errx(EXIT_FAILURE, "Error --note option : Malformed syntax for -a");
            nextNotSpace(&token);
            break;
        case 't':
            note.text = parseText(&token, end);
            if (!note.text)
                errx(EXIT_FAILURE, "Error --note option : Malformed syntax for -t");
            break;
        case 'c':
            c = strtok_r(token, ",", &savePtr);

            int numberColors = 0;

            while (c) {
                token = c;

                const int color = optionsParseRequireRange(
                        optionsParseRequiredNumber(c), 0 ,255);

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

            if (numberColors > 4)
                 warnx("Warning --note option : Malformed syntax for -c");
            break;
        default:
            errx(EXIT_FAILURE, "Error --note option : unknown option: '-%c'", type);
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

    imlib_context_set_color(note.color.r,
        note.color.g,
        note.color.b,
        note.color.a);

    imlib_text_draw(note.x, note.y, note.text);
}

static void loadFont(void)
{
    imFont = imlib_load_font(note.font);

    if (!imFont) {
        warnx("Error --note option : Failed to load fontname: %s", note.font);
        scrotNoteFree();
        exit(EXIT_FAILURE);
    }
}

static char *parseText(char **token, char *const end)
{
    assert(NULL != *token);
    assert(NULL != end);

    if (**token != '\'')
        return NULL;

    (*token)++;

    char *begin = *token;

    while ((*token != end) && **token != '\'')
        (*token)++;

    ptrdiff_t length = (*token - begin);

    if (length == 0)
        return NULL;

    return strndup(begin, length);
}
