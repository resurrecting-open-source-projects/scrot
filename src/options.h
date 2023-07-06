/* options.h

Copyright 1999-2000 Tom Gilbert <tom@linuxbrit.co.uk,
                                  gilbertt@linuxbrit.co.uk,
                                  scrot_sucks@linuxbrit.co.uk>
Copyright 2009      James Cameron <quozl@us.netrek.org>
Copyright 2010      Ibragimov Rinat <ibragimovrinat@mail.ru>
Copyright 2017      Stoney Sauce <stoneysauce@gmail.com>
Copyright 2019-2023 Daniel T. Borelli <danieltborelli@gmail.com>
Copyright 2020      Sean Brennan <zettix1@gmail.com>
Copyright 2021-2023 Guilherme Janczak <guilherme.janczak@yandex.com>
Copyright 2021      IFo Hancroft <contact@ifohancroft.com>
Copyright 2021      Peter Wu <peterwu@hotmail.com>
Copyright 2021      Wilson Smith <01wsmith+gh@gmail.com>
Copyright 2022      Zev Weiss <zev@bewilderbeest.net>
Copyright 2023      NRK <nrk@disroot.org>

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

#ifndef H_OPTIONS
#define H_OPTIONS

#include <time.h>

#include "scrot_selection.h"

// General purpose enum
enum Direction {
    // see scrot.c:stalkImageConcat(...)
    HORIZONTAL,
    VERTICAL,
};

enum ShotMode {
    MODE_SCREEN, /* first is the default one */
    MODE_WINDOW,
    MODE_AUTOSEL,
    MODE_MONITOR,
    MODE_STACK,
    MODE_MULTIDISP,
    MODE_FOCUSED,
    MODE_SELECT,
};

struct ScrotOptions {
    enum ShotMode mode;
    int delay;
    struct timespec delayStart;
    int quality;
    int compression;
    enum { THUMB_DISABLED, THUMB_PERCENT, THUMB_RES } thumb;
    int thumbPercent;
    int thumbW;
    int thumbH;
    enum LineMode lineMode;
    int lineStyle;
    int lineWidth;
    int lineOpacity;
    enum Direction stackDirection;
    const char *format;
    const char *lineColor;
    const char *outputFile;
    char *thumbFile;
    const char *exec;
    const char *display;
    char *note;
    Window windowId;
    const char *windowClassName;
    const char *script;
    int autoselectX;
    int autoselectY;
    int autoselectH;
    int autoselectW;
    SelectionMode selection;
    int monitor;
    bool delaySelection;
    bool countdown;
    bool border;
    bool silent;
    bool pointer;
    bool overwrite;
    bool freeze;
    bool ignoreKeyboard;
};

extern struct ScrotOptions opt;

void optionsParse(int, char **);
void optionsParseAutoselect(char *);
long long optionsParseNum(const char *, long long, long long,
    const char *[static 1]);

#endif /* !defined(H_OPTIONS) */
