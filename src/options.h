/* options.h

Copyright 1999-2000 Tom Gilbert <tom@linuxbrit.co.uk,
                                  gilbertt@linuxbrit.co.uk,
                                  scrot_sucks@linuxbrit.co.uk>
Copyright 2009      James Cameron <quozl@us.netrek.org>
Copyright 2010      Ibragimov Rinat <ibragimovrinat@mail.ru>
Copyright 2017      Stoney Sauce <stoneysauce@gmail.com>
Copyright 2019-2021 Daniel T. Borelli <daltomi@disroot.org>
Copyright 2020      Sean Brennan <zettix1@gmail.com>
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

#include "structs.h"

#pragma once

struct __ScrotOptions {
    int delay;
    int countdown;
    int select;
    int focused;
    int quality;
    int border;
    int silent;
    int multidisp;
    int thumb;
    int thumbWidth;
    int thumbHeight;
    int pointer;
    int freeze;
    int overwrite;
    int lineStyle;
    int lineWidth;
    int lineOpacity;
    int stack;
    char* lineColor;
    char* lineMode;
    char* outputFile;
    char* thumbFile;
    char* exec;
    char* display;
    char* note;
    char* windowClassName;
    char* script;
    int autoselect;
    int autoselectX;
    int autoselectY;
    int autoselectH;
    int autoselectW;
};

void optionsParse(int, char**);
char* nameThumbnail(char*);
void optionsParseFileName(char*);
void optionsParseThumbnail(char*);
void optionsParseAutoselect(char*);
void optionsParseDisplay(char*);
void optionsParseNote(char*);
int optionsParseRequiredNumber(char*);
int optionsCompareWindowClassName(const char*);
int optionsParseRequireRange(int, int, int);
extern ScrotOptions opt;
