/* options.c

Copyright 1999-2000 Tom Gilbert <tom@linuxbrit.co.uk,
                                  gilbertt@linuxbrit.co.uk,
                                  scrot_sucks@linuxbrit.co.uk>
Copyright 2008      William Vera <billy@billy.com.mx>
Copyright 2009      George Danchev <danchev@spnet.net>
Copyright 2009      James Cameron <quozl@us.netrek.org>
Copyright 2010      Ibragimov Rinat <ibragimovrinat@mail.ru>
Copyright 2017      Stoney Sauce <stoneysauce@gmail.com>
Copyright 2019      Daniel Lublin <daniel@lublin.se>
Copyright 2019-2021 Daniel T. Borelli <danieltborelli@gmail.com>
Copyright 2019      Jade Auer <jade@trashwitch.dev>
Copyright 2020      Sean Brennan <zettix1@gmail.com>
Copyright 2021      Christopher R. Nelson <christopher.nelson@languidnights.com>
Copyright 2021-2022 Guilherme Janczak <guilherme.janczak@yandex.com>
Copyright 2021      IFo Hancroft <contact@ifohancroft.com>
Copyright 2021      Peter Wu <peterwu@hotmail.com>
Copyright 2021      Wilson Smith <01wsmith+gh@gmail.com>

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
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "note.h"
#include "options.h"
#include "scrot.h"
#include "scrot_selection.h"
#include "util.h"

#define STR_LEN_MAX_FILENAME(msg, fileName) do {                \
    if (strlen((fileName)) > MAX_FILENAME) {                    \
        errx(EXIT_FAILURE, #msg " filename too long, must be "  \
            "less than %d characters", MAX_FILENAME);           \
    }                                                           \
} while(0)

#define checkMaxOutputFileName(fileName) \
    STR_LEN_MAX_FILENAME(output, (fileName))

#define checkMaxInputFileName(fileName) \
    STR_LEN_MAX_FILENAME(input, (fileName))

enum {
    MAX_LEN_WINDOW_CLASS_NAME = 80, //characters
    MAX_FILENAME = 256, // characters
    MAX_DISPLAY_NAME = 256, // characters
};

struct ScrotOptions opt = {
    .quality = 75,
    .lineStyle = LineSolid,
    .lineWidth = 1,
    .lineOpacity = SELECTION_OPACITY_DEFAULT,
    .lineMode = LINE_MODE_S_CLASSIC,
    .stackDirection = HORIZONTAL,
};

static void showUsage(void);
static void showVersion(void);

int optionsParseRequiredNumber(const char *str)
{
    assert(NULL != str); // fix yout caller function,
                         //  the user does not impose this behavior
    char *end = NULL;
    long ret = 0L;
    errno = 0;

    ret = strtol(str, &end, 10);

    if (errno)
        goto range_error;

    if (str == end)
        errx(EXIT_FAILURE, "the option is not a number: %s", end);

    if (ret > INT_MAX || ret < INT_MIN) {
        errno = ERANGE;
        goto range_error;
    }

    return ret;

range_error:
    err(EXIT_FAILURE, "error strtol");
}

static int nonNegativeNumber(int number)
{
    return (number < 0) ? 0 : number;
}

int optionsParseRequireRange(int n, int lo, int hi)
{
    return (n < lo ? lo : n > hi ? hi : n);
}

static bool optionsParseIsString(const char *const str)
{
    return (str && (str[0] != '\0'));
}

static void optionsParseStack(const char *optarg)
{
    // the suboption it's optional
    if (!optarg) {
        opt.stackDirection = HORIZONTAL;
        return;
    }
    const char *value = strchr(optarg, '=');

    if (value)
        ++value;
    else
        value = optarg;

    if (*value == 'v')
        opt.stackDirection = VERTICAL;
    else if (*value == 'h')
        opt.stackDirection = HORIZONTAL;
    else {
        errx(EXIT_FAILURE, "option --stack: Unknown value for suboption '%s'",
             value);
    }
}

static void optionsParseSelection(const char *optarg)
{
    // the suboption it's optional
    if (!optarg) {
        opt.selection.mode = SELECTION_MODE_CAPTURE;
        return;
    }

    const char *value = strchr(optarg, '=');

    if (value)
        ++value;
    else
        value = optarg;

    if (!strncmp(value, SELECTION_MODE_S_CAPTURE, SELECTION_MODE_L_CAPTURE)) {
        opt.selection.mode = SELECTION_MODE_CAPTURE;
        return; /* it has no parameter */
    }
    else if (!strncmp(value, SELECTION_MODE_S_HIDE, SELECTION_MODE_L_HIDE)) {
        opt.selection.mode = SELECTION_MODE_HIDE;
        value += SELECTION_MODE_L_HIDE;
    }
    else if (!strncmp(value, SELECTION_MODE_S_HOLE, SELECTION_MODE_L_HOLE)) {
        opt.selection.mode = SELECTION_MODE_HOLE;
    }
    else if (!strncmp(value, SELECTION_MODE_S_BLUR, SELECTION_MODE_L_BLUR)) {
        opt.selection.mode = SELECTION_MODE_BLUR;
        opt.selection.paramNum = SELECTION_MODE_BLUR_DEFAULT;
        value += SELECTION_MODE_L_BLUR;
    }
    else {
        errx(EXIT_FAILURE, "option --select: Unknown value for suboption '%s'",
             value);
    }

    if (opt.selection.mode & SELECTION_MODE_NOT_NEED_PARAM)
        return;

    if (*value != SELECTION_MODE_SEPARATOR)
        return;

    if (*(++value) == '\0')
        errx(EXIT_FAILURE, "option --select: Invalid parameter.");

    if (opt.selection.mode == SELECTION_MODE_BLUR) {
        int const num = nonNegativeNumber(optionsParseRequiredNumber(value));

        opt.selection.paramNum = optionsParseRequireRange(num,
            SELECTION_MODE_BLUR_MIN, SELECTION_MODE_BLUR_MAX);

    } else { // SELECTION_MODE_HIDE

        checkMaxInputFileName(value);

        opt.selection.paramStr = estrdup(value);
    }
}

static void optionsParseLine(char *optarg)
{
    enum {
        Style = 0,
        Width,
        Color,
        Opacity,
        Mode
    };

    char *const token[] = {
        [Style] = "style",
        [Width] = "width",
        [Color] = "color",
        [Opacity] = "opacity",
        [Mode] = "mode",
        NULL
    };

    char *subopts = optarg;
    char *value = NULL;

    while (*subopts != '\0') {
        switch (getsubopt(&subopts, token, &value)) {
        case Style:
            if (!optionsParseIsString(value)) {
                errx(EXIT_FAILURE, "Missing value for suboption '%s'",
                    token[Style]);
            }

            if (!strncmp(value, "dash", 4))
                opt.lineStyle = LineOnOffDash;
            else if (!strncmp(value, "solid", 5))
                opt.lineStyle = LineSolid;
            else {
                errx(EXIT_FAILURE, "Unknown value for suboption '%s': %s",
                    token[Style], value);
            }
            break;
        case Width:
            if (!optionsParseIsString(value)) {
                errx(EXIT_FAILURE, "Missing value for suboption '%s'",
                    token[Width]);
            }

            opt.lineWidth = optionsParseRequiredNumber(value);

            if (opt.lineWidth <= 0 || opt.lineWidth > 8) {
                errx(EXIT_FAILURE, "Value of the range (1..8) for "
                    "suboption '%s': %d", token[Width], opt.lineWidth);
            }
            break;
        case Color:
            if (!optionsParseIsString(value)) {
                errx(EXIT_FAILURE, "Missing value for suboption '%s'",
                    token[Color]);
            }

            opt.lineColor = estrdup(value);
            break;
        case Mode:
            if (!optionsParseIsString(value)) {
                errx(EXIT_FAILURE, "Missing value for suboption '%s'",
                    token[Mode]);
            }

            bool isValidMode = !strncmp(value, LINE_MODE_S_CLASSIC, LINE_MODE_L_CLASSIC);

            isValidMode = isValidMode || !strncmp(value, LINE_MODE_S_EDGE, LINE_MODE_L_EDGE);

            if (!isValidMode) {
                errx(EXIT_FAILURE, "Unknown value for suboption '%s': %s",
                    token[Mode], value);
            }

            opt.lineMode = estrdup(value);
            break;
        case Opacity:
            if (!optionsParseIsString(value)) {
                errx(EXIT_FAILURE, "Missing value for suboption '%s'",
                    token[Opacity]);
            }
            opt.lineOpacity = optionsParseRequiredNumber(value);
            break;
        default:
            errx(EXIT_FAILURE, "No match found for token: '%s'", value);
            break;
        }
    } /* while */
}

static void optionsParseWindowClassName(const char *windowClassName)
{
    assert(windowClassName != NULL);

    if (windowClassName[0] != '\0')
        opt.windowClassName = strndup(windowClassName, MAX_LEN_WINDOW_CLASS_NAME);
}

static bool accessFileOk(const char *const pathName)
{
    errno = 0;
    return (0 == access(pathName, W_OK));
}

static char *getPathOfStdout(void)
{
    char path[16] = {"/dev/stdout"};
    const size_t len = sizeof(path);

    if (!accessFileOk(path)) {

        snprintf(path, len, "/dev/fd/%d", STDOUT_FILENO);

        if (!accessFileOk(path)) {

            snprintf(path, len, "/proc/self/fd/%d", STDOUT_FILENO);

            if (!accessFileOk(path)) {
                // We quit because imlib2 will fail later anyway.
                err(EXIT_FAILURE, "access to stdout failed");
            }
        }
    }
    return strndup(path, len);
}

void optionsParse(int argc, char *argv[])
{
    static char stropts[] = "a:ofipbcd:e:hmq:s::t:uvzn:l:D:k::C:S:F:";

    static struct option lopts[] = {
        /* actions */
        { "help", no_argument, 0, 'h' },
        { "version", no_argument, 0, 'v' },
        { "count", no_argument, 0, 'c' },
        { "focused", no_argument, 0, 'u' },
        { "focussed", no_argument, 0, 'u' }, /* macquarie dictionary has both spellings */
        { "border", no_argument, 0, 'b' },
        { "multidisp", no_argument, 0, 'm' },
        { "silent", no_argument, 0, 'z' },
        { "pointer", no_argument, 0, 'p' },
        { "ignorekeyboard", no_argument, 0, 'i' },
        { "freeze", no_argument, 0, 'f' },
        { "overwrite", no_argument, 0, 'o' },
        /* toggles */
        { "stack", optional_argument, 0, 'k' },
        { "select", optional_argument, 0, 's' },
        { "thumb", required_argument, 0, 't' },
        { "delay", required_argument, 0, 'd' },
        { "quality", required_argument, 0, 'q' },
        { "exec", required_argument, 0, 'e' },
        { "autoselect", required_argument, 0, 'a' },
        { "display", required_argument, 0, 'D' },
        { "note", required_argument, 0, 'n' },
        { "line", required_argument, 0, 'l' },
        { "class", required_argument, 0, 'C' },
        { "script", required_argument, 0, 'S' },
        { "file", required_argument, 0, 'F' },
        { 0, 0, 0, 0 }
    };
    int optch = 0, cmdx = 0;

    /* Now to pass some optionarinos */
    while ((optch = getopt_long(argc, argv, stropts, lopts, &cmdx)) != EOF) {
        switch (optch) {
        case 0:
            break;
        case 'h':
            showUsage();
            break;
        case 'v':
            showVersion();
            break;
        case 'b':
            opt.border = 1;
            break;
        case 'd':
            opt.delay = nonNegativeNumber(optionsParseRequiredNumber(optarg));
            break;
        case 'e':
            opt.exec = estrdup(optarg);
            break;
        case 'm':
            opt.multidisp = 1;
            break;
        case 'q':
            opt.quality = optionsParseRequiredNumber(optarg);
            break;
        case 's':
            optionsParseSelection(optarg);
            break;
        case 'u':
            opt.focused = 1;
            break;
        case 'c':
            opt.countdown = 1;
            break;
        case 't':
            optionsParseThumbnail(optarg);
            break;
        case 'z':
            opt.silent = 1;
            break;
        case 'p':
            opt.pointer = 1;
            break;
        case 'i':
            opt.ignoreKeyboard = 1;
            break;
        case 'f':
            opt.freeze = 1;
            break;
        case 'o':
            opt.overwrite = 1;
            break;
        case 'a':
            optionsParseAutoselect(optarg);
            break;
        case 'D':
            optionsParseDisplay(optarg);
            break;
        case 'n':
            optionsParseNote(optarg);
            break;
        case 'l':
            optionsParseLine(optarg);
            break;
        case 'k':
            opt.stack = 1;
            optionsParseStack(optarg);
            break;
        case 'C':
            optionsParseWindowClassName(optarg);
            break;
        case 'S':
            opt.script = estrdup(optarg);
            break;
        case 'F':
            optionsParseFileName(optarg);
            break;
        case '?':
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    /* Now the leftovers, which must be files */
    while (optind < argc) {
        /* If recursive is NOT set, but the only argument is a directory
           name, we grab all the files in there, but not subdirs */
        if (!opt.outputFile) {
            optionsParseFileName(argv[optind++]);

            const bool redirectChar = ( opt.outputFile[0] == '-'
                                        && opt.outputFile[1] == '\0');
            if (redirectChar) {
                free(opt.outputFile);
                opt.outputFile = getPathOfStdout();
                opt.overwrite = 1;
                opt.thumb = 0;
            }
        } else
            warnx("unrecognised option %s", argv[optind++]);
    }

    /* So that we can safely be called again */
    optind = 1;
}

static void showUsage(void)
{
    fputs(/* Check that everything lines up after any changes. */
        "usage:  " PACKAGE " [-bcfhimopuvz] [-a X,Y,W,H] [-C NAME] [-D DISPLAY]\n"
        "              [-d SEC] [-e CMD] [-F FILE] [-k OPT] [-l STYLE] [-n OPTS]\n"
        "              [-q NUM] [-S CMD] [-s OPTS] [-t NUM | GEOM] [FILE]\n",
        stdout);
    exit(0);
}

static void showVersion(void)
{
    printf(PACKAGE " version " VERSION "\n");
    exit(0);
}

char *optionsNameThumbnail(const char *name)
{
    const char *const thumbSuffix = "-thumb";
    const size_t thumbSuffixLength = 7;
    const size_t newNameLength = strlen(name) + thumbSuffixLength;
    char *newName = calloc(1, newNameLength);

    if (!newName)
        err(EXIT_FAILURE, "Unable to allocate thumbnail");

    const char *const extension = strrchr(name, '.');

    if (extension) {
        /* We add one so length includes '\0'*/
        const ptrdiff_t nameLength = (extension - name) + 1;
        strlcpy(newName, name, nameLength);
        strlcat(newName, thumbSuffix, newNameLength);
        strlcat(newName, extension, newNameLength);
    } else
        snprintf(newName, newNameLength, "%s%s", name, thumbSuffix);

    return newName;
}

void optionsParseAutoselect(char *optarg)
{
    char *token;
    const char tokenDelimiter[2] = ",";
    int dimensions[4];
    int i = 0;

    if (strchr(optarg, ',')) { /* geometry dimensions must be in format x,y,w,h   */
        dimensions[i++] = optionsParseRequiredNumber(strtok(optarg, tokenDelimiter));
        while ((token = strtok(NULL, tokenDelimiter)))
            dimensions[i++] = optionsParseRequiredNumber(token);
        opt.autoselect = 1;
        opt.autoselectX = dimensions[0];
        opt.autoselectY = dimensions[1];
        opt.autoselectW = dimensions[2];
        opt.autoselectH = dimensions[3];

        if (i != 4)
            errx(EXIT_FAILURE, "option 'autoselect' require 4 arguments");
    } else
        errx(EXIT_FAILURE, "invalid format for option -- 'autoselect'");
}

void optionsParseDisplay(char *optarg)
{
    opt.display = strndup(optarg, MAX_DISPLAY_NAME);
    if (!opt.display)
        err(EXIT_FAILURE, "Unable to allocate display");
}

void optionsParseThumbnail(char *optarg)
{
    char *token;

    if (strchr(optarg, 'x')) { /* We want to specify the geometry */
        token = strtok(optarg, "x");
        opt.thumbWidth = optionsParseRequiredNumber(token);
        token = strtok(NULL, "x");
        if (token) {
            opt.thumbWidth = optionsParseRequiredNumber(optarg);
            opt.thumbHeight = optionsParseRequiredNumber(token);

            if (opt.thumbWidth < 0)
                opt.thumbWidth = 1;
            if (opt.thumbHeight < 0)
                opt.thumbHeight = 1;

            if (!opt.thumbWidth && !opt.thumbHeight)
                opt.thumb = 0;
            else
                opt.thumb = 1;
        }
    } else {
        opt.thumb = optionsParseRequireRange(
                        optionsParseRequiredNumber(optarg), 1, 100);
    }

}

void optionsParseFileName(const char *optarg)
{
    checkMaxOutputFileName(optarg);
    opt.outputFile = estrdup(optarg);
}

void optionsParseNote(char *optarg)
{
    opt.note = estrdup(optarg);

    if (!opt.note)
        return;

    if (opt.note[0] == '\0')
        errx(EXIT_FAILURE, "Required arguments for --note.");

    scrotNoteNew(opt.note);
}

/*
Return:
    0 : It does not match
    1 : If it matches
*/
int optionsCompareWindowClassName(const char *targetClassName)
{
    assert(targetClassName != NULL);
    assert(opt.windowClassName != NULL);
    return !!(!strncmp(targetClassName, opt.windowClassName, MAX_LEN_WINDOW_CLASS_NAME - 1));
}
