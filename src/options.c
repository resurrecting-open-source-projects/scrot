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
Copyright 2019-2023 Daniel T. Borelli <danieltborelli@gmail.com>
Copyright 2019      Jade Auer <jade@trashwitch.dev>
Copyright 2020      Sean Brennan <zettix1@gmail.com>
Copyright 2021      Christopher R. Nelson <christopher.nelson@languidnights.com>
Copyright 2021-2023 Guilherme Janczak <guilherme.janczak@yandex.com>
Copyright 2021      IFo Hancroft <contact@ifohancroft.com>
Copyright 2021      Peter Wu <peterwu@hotmail.com>
Copyright 2021      Wilson Smith <01wsmith+gh@gmail.com>
Copyright 2022      Zev Weiss <zev@bewilderbeest.net>
Copyright 2023-2024 NRK <nrk@disroot.org>

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

#include <err.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "note.h"
#include "options.h"
#include "scrot.h"
#include "scrot_selection.h"
#include "util.h"

#ifndef PACKAGE_NAME
    #define PACKAGE_NAME "scrot"
#endif
#ifndef PACKAGE_VERSION
    #define PACKAGE_VERSION "unknown"
#endif

static char defaultOutputFile[] = "%Y-%m-%d-%H%M%S_$wx$h_scrot.$F";
struct ScrotOptions opt = {
    .quality = 75,
    .compression = 7,
    .lineStyle = LineSolid,
    .lineWidth = 1,
    .lineOpacity = SELECTION_OPACITY_DEFAULT,
    .stackDirection = HORIZONTAL,
    .outputFile = defaultOutputFile,
    .lineColor = "gray",
};

enum { /* long opt only */
    /* ensure these don't collide with single byte opts. */
    OPT_FORMAT = UCHAR_MAX + 1,
    OPT_LIST_OPTS,
};
static const char stropts[] = "a:bC:cD:d:e:F:fhik::l:M:mn:opq:S:s::t:uvw:Z:z";
// NOTE: make sure lopts and opt_description indexes are kept in sync
static const struct option lopts[] = {
    {"autoselect",      required_argument,  NULL,   'a'},
    {"border",          no_argument,        NULL,   'b'},
    {"class",           required_argument,  NULL,   'C'},
    {"count",           no_argument,        NULL,   'c'},
    {"display",         required_argument,  NULL,   'D'},
    {"delay",           required_argument,  NULL,   'd'},
    {"exec",            required_argument,  NULL,   'e'},
    {"file",            required_argument,  NULL,   'F'},
    {"freeze",          no_argument,        NULL,   'f'},
    {"help",            no_argument,        NULL,   'h'},
    {"ignorekeyboard",  no_argument,        NULL,   'i'},
    {"stack",           optional_argument,  NULL,   'k'},
    {"line",            required_argument,  NULL,   'l'},
    {"monitor",         required_argument,  NULL,   'M'},
    {"multidisp",       no_argument,        NULL,   'm'},
    {"note",            required_argument,  NULL,   'n'},
    {"overwrite",       no_argument,        NULL,   'o'},
    {"pointer",         no_argument,        NULL,   'p'},
    {"quality",         required_argument,  NULL,   'q'},
    {"script",          required_argument,  NULL,   'S'},
    {"select",          optional_argument,  NULL,   's'},
    {"thumb",           required_argument,  NULL,   't'},
    {"focused",         no_argument,        NULL,   'u'},
    /* macquarie dictionary has both spellings */
    {"focussed",        no_argument,        NULL,   'u'},
    {"version",         no_argument,        NULL,   'v'},
    {"window",          required_argument,  NULL,   'w'},
    {"compression",     required_argument,  NULL,   'Z'},
    {"silent",          no_argument,        NULL,   'z'},
    {"format",          required_argument,  NULL, OPT_FORMAT},
    {"list-options",    optional_argument,  NULL, OPT_LIST_OPTS},
    {0}
};
static const char OPT_DEPRECATED[] = "";
static const struct option_desc {
    const char *description, *arg_description;
} opt_description[] = {
    /* a */  { "autoselect provided region", "x,y,w,h" },
    /* b */  { "capture the window borders as well", "" },
    /* C */  { "capture specified window class", "NAME" },
    /* c */  { "display a countdown for delay", "" },
    /* D */  { "capture specified display", "DISPLAY" },
    /* d */  { "add delay before screenshot", "[b]SEC" },
    /* e */  { "execute command on saved image", "CMD" },
    /* F */  { "specify output file", "FILE" },
    /* f */  { "freeze the screen when -s is used", "" },
    /* h */  { "display help and exit", "" },
    /* i */  { "ignore keyboard", "" },
    /* k */  { "capture overlapped window and join them", "v|h" },
    /* l */  { "specify the style of the selection line", "STYLE" },
    /* M */  { "capture monitor", "NUM" },
    /* m */  { "capture all monitors", "" },
    /* n */  { OPT_DEPRECATED, OPT_DEPRECATED },
    /* o */  { "overwrite the output file if needed", "" },
    /* p */  { "capture the mouse pointer as well", "" },
    /* q */  { "image quality", "NUM" },
    /* S */  { OPT_DEPRECATED, OPT_DEPRECATED },
    /* s */  { "interactively select a region to capture", "OPTS" },
    /* t */  { "also generate a thumbnail", "% | WxH" },
    /* u */  { "capture the currently focused window", "" },
    /* u */  { "capture the currently focused window", "" },
    /* v */  { "output version and exit", "" },
    /* w */  { "X window ID to capture", "WID" },
    /* Z */  { "image compression level", "LVL" },
    /* z */  { "prevent beeping", "" },
    /* OPT_FORMAT */     { "specify output file format", "FMT" },
    /* OPT_LIST_OPTS */  { "list all options", "human|tsv" },
};

static void showUsage(void);
static void showOptions(bool human);
static void showVersion(void);
static long long optionsParseNumBase(const char *, long long, long long,
    const char *[static 1], int);
static void optionsParseThumbnail(char *);
static char *optionsNameThumbnail(const char *);

long long optionsParseNum(const char *str, long long min, long long max,
    const char *errmsg[static 1])
{
    return optionsParseNumBase(str, min, max, errmsg, 10);
}

/* optionsParseNumBase: "string to number" function.
 *
 * Parses the string representation of an integer in str, and simultaneously
 * ensures that it is >= min and <= max. The string representation must be of
 * base `base`.
 *
 * Returns the integer and sets *errmsg to NULL on success.
 * Returns 0 and sets *errmsg to a pointer to a string containing the
 * reason why the number can't be parsed on error.
 */
static long long optionsParseNumBase(const char *str, long long min,
    long long max, const char *errmsg[static 1], int base)
{
    char *end = NULL;
    long long rval;

    if (str == NULL) {
        *errmsg = "missing";
        return 0;
    }
    *errmsg = NULL;

    errno = 0;
    rval = strtoll(str, &end, base);
    if (errno == ERANGE) {
        *errmsg = "not representable";
    } else if (*str == '\0') {
        *errmsg = "the null string";
    } else if (*end != '\0') {
        *errmsg = "not an integer";
    } else if (rval < min) {
        /*
         * rval could be set to 0 due to strtoll() returning error and this
         * could be smaller than min or larger than max. To make sure we don't
         * return the wrong error message, put min/max checks after everything
         * else.
         */
        *errmsg = min == 0 ? "negative" : "too small";
    } else if (rval > max) {
        *errmsg = "too large";
    }

    return (*errmsg ? 0 : rval);
}

static bool optionsParseIsString(const char *const str)
{
    return (str && (str[0] != '\0'));
}

static void optionsParseStack(const char *optarg)
{
    if (!optarg /* the suboption is optional */ || strcmp(optarg, "h") == 0)
        opt.stackDirection = HORIZONTAL;
    else if (strcmp(optarg, "v") == 0)
        opt.stackDirection = VERTICAL;
    else {
        errx(EXIT_FAILURE, "option --stack: Unknown value for suboption '%s'",
             optarg);
    }
}

static void optionsParseSelection(const char *optarg)
{
    // the suboption it's optional
    if (!optarg) {
        opt.selection.mode = SELECTION_MODE_CAPTURE;
        return;
    }
    const char *value = optarg;

    if (!strncmp(value, SELECTION_MODE_S_CAPTURE, SELECTION_MODE_L_CAPTURE)) {
        opt.selection.mode = SELECTION_MODE_CAPTURE;
        return; /* it has no parameter */
    } else if (!strncmp(value, SELECTION_MODE_S_HIDE, SELECTION_MODE_L_HIDE)) {
        opt.selection.mode = SELECTION_MODE_HIDE;
        value += SELECTION_MODE_L_HIDE;
    } else if (!strncmp(value, SELECTION_MODE_S_HOLE, SELECTION_MODE_L_HOLE)) {
        opt.selection.mode = SELECTION_MODE_HOLE;
    } else if (!strncmp(value, SELECTION_MODE_S_BLUR, SELECTION_MODE_L_BLUR)) {
        opt.selection.mode = SELECTION_MODE_BLUR;
        opt.selection.blur = SELECTION_MODE_BLUR_DEFAULT;
        value += SELECTION_MODE_L_BLUR;
    } else {
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
        const char *errmsg;
        opt.selection.blur = optionsParseNum(value,
            SELECTION_MODE_BLUR_MIN, SELECTION_MODE_BLUR_MAX, &errmsg);
        if (errmsg)
            errx(EXIT_FAILURE, "option --select: '%s' is %s", value, errmsg);
    } else { // SELECTION_MODE_HIDE
        opt.selection.fileName = value;
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
    const char *errmsg;

    while (*subopts != '\0') {
        switch (getsubopt(&subopts, token, &value)) {
        case Style:
            if (!optionsParseIsString(value)) {
                errx(EXIT_FAILURE, "Missing value for suboption '%s'",
                    token[Style]);
            }

            if (!strcmp(value, "dash"))
                opt.lineStyle = LineOnOffDash;
            else if (!strcmp(value, "solid"))
                opt.lineStyle = LineSolid;
            else {
                errx(EXIT_FAILURE, "Unknown value for suboption '%s': %s",
                    token[Style], value);
            }
            break;
        case Width:
            opt.lineWidth = optionsParseNum(value, 1, 8, &errmsg);
            if (errmsg) {
                if (value == NULL)
                    value = "(null)";
                errx(EXIT_FAILURE, "option --line: suboption '%s': '%s' is %s",
                    token[Width], value, errmsg);
            }
            break;
        case Color:
            if (!optionsParseIsString(value)) {
                errx(EXIT_FAILURE, "Missing value for suboption '%s'",
                    token[Color]);
            }

            opt.lineColor = value;
            break;
        case Mode:
            if (!optionsParseIsString(value)) {
                errx(EXIT_FAILURE, "Missing value for suboption '%s'",
                    token[Mode]);
            }

            if (!strcmp(value, "auto"))
                opt.lineMode = LINE_MODE_AUTO;
            else if (!strcmp(value, "classic"))
                opt.lineMode = LINE_MODE_CLASSIC;
            else if (!strcmp(value, "edge"))
                opt.lineMode = LINE_MODE_EDGE;
            else {
                errx(EXIT_FAILURE, "Unknown value for suboption '%s': %s",
                    token[Mode], value);
            }
            break;
        case Opacity:
            opt.lineOpacity = optionsParseNum(value,
                SELECTION_OPACITY_MIN, SELECTION_OPACITY_MAX, &errmsg);
            if (errmsg) {
                if (value == NULL)
                    value = "(null)";
                errx(EXIT_FAILURE, "option --line: suboption %s: '%s' is %s",
                    token[Opacity], value, errmsg);
            }
            break;
        default:
            errx(EXIT_FAILURE, "No match found for token: '%s'", value);
            break;
        }
    } /* while */
}

static const char *getPathOfStdout(void)
{
    const char *paths[] = { "/dev/stdout", "/dev/fd/1", "/proc/self/fd/1" };

    for (size_t i = 0; i < ARRAY_COUNT(paths); ++i) {
        if (access(paths[i], W_OK) == 0)
            return paths[i];
    }
    err(EXIT_FAILURE, "access to stdout failed");
    return 0; /* silence tcc warning */
}

void optionsParse(int argc, char *argv[])
{
    int optch;
    const char *errmsg;
    bool FFlagSet = false;
    const char *note = NULL;

    /* Now to pass some optionarinos */
    while ((optch = getopt_long(argc, argv, stropts, lopts, NULL)) != -1) {
        switch (optch) {
        case 'a':
            opt.mode = MODE_AUTOSEL;
            optionsParseAutoselect(optarg);
            break;
        case 'b':
            opt.border = true;
            break;
        case 'C':
            opt.windowClassName = optarg;
            break;
        case 'c':
            opt.countdown = true;
            break;
        case 'D':
            opt.display = optarg;
            break;
        case 'd':
            opt.delaySelection = *optarg == 'b';
            if (opt.delaySelection)
                ++optarg;
            /* NOTE: div 1000 so that converting to milliseconds doesn't overflow */
            opt.delay = optionsParseNum(optarg, 0, INT_MAX/1000, &errmsg);
            if (errmsg) {
                errx(EXIT_FAILURE, "option --delay: '%s' is %s", optarg,
                    errmsg);
            }
            break;
        case 'e':
            opt.exec = optarg;
            break;
        case 'F':
            FFlagSet = true;
            opt.outputFile = optarg;
            break;
        case 'f':
            opt.freeze = true;
            break;
        case 'h':
            showUsage();
            break;
        case 'i':
            opt.ignoreKeyboard = true;
            break;
        case 'k':
            opt.mode = MODE_STACK;
            optionsParseStack(optarg);
            break;
        case 'l':
            optionsParseLine(optarg);
            break;
        case 'M':
            opt.mode = MODE_MONITOR;
            opt.monitor = optionsParseNum(optarg, 0, INT_MAX, &errmsg);
            if (errmsg) {
                errx(EXIT_FAILURE, "option --monitor: '%s' is %s", optarg,
                    errmsg);
            }
            break;
        case 'm':
            opt.mode = MODE_MULTIDISP;
            break;
        case 'n':
            if (optarg[0] == '\0')
                errx(EXIT_FAILURE, "Required arguments for --note.");
            note = optarg;
            break;
        case 'o':
            opt.overwrite = true;
            break;
        case 'p':
            opt.pointer = true;
            break;
        case 'q':
            opt.quality = optionsParseNum(optarg, 1, 100, &errmsg);
            if (errmsg) {
                errx(EXIT_FAILURE, "option --quality: '%s' is %s", optarg,
                    errmsg);
            }
            break;
        case 'S':
            opt.script = optarg;
            break;
        case 's':
            opt.mode = MODE_SELECT;
            optionsParseSelection(optarg);
            break;
        case 't':
            optionsParseThumbnail(optarg);
            break;
        case 'u':
            opt.mode = MODE_FOCUSED;
            break;
        case 'v':
            showVersion();
            break;
        case 'w':
            opt.mode = MODE_WINDOW;
            opt.windowId = optionsParseNumBase(optarg, None/*0L*/, LONG_MAX, &errmsg, 0);
            if (errmsg) {
                errx(EXIT_FAILURE, "option --window: '%s' is %s", optarg,
                    errmsg);
            }
            break;
        case 'Z':
            opt.compression = optionsParseNum(optarg, 0, 9, &errmsg);
            if (errmsg) {
                errx(EXIT_FAILURE, "option --compression: '%s' is %s", optarg,
                    errmsg);
            }
            break;
        case 'z':
            opt.silent = true;
            break;
        case OPT_FORMAT:
            opt.format = optarg;
            break;
        case OPT_LIST_OPTS:
            if (optarg == NULL || strcmp(optarg, "human") == 0)
                showOptions(true);
            else if (strcmp(optarg, "tsv") == 0)
                showOptions(false);
            else {
                errx(EXIT_FAILURE,
                    "unknown argument for --list-options: `%s`", optarg);
            }
            break;
        default:
            exit(EXIT_FAILURE);
        }
    }
    argv += optind;

    if (!FFlagSet && *argv) {
        opt.outputFile = *argv;
        argv++;
    }

    for (; *argv; ++argv)
        warnx("ignoring extraneous non-option argument: %s", *argv);

    if (!opt.format) {
        char *ext;
        size_t extLength = scrotHaveFileExtension(opt.outputFile, &ext);
        if (extLength == 0 || opt.outputFile == defaultOutputFile)
            opt.format = "png";
        else
            opt.format = ext+1;
    }

    if (strcmp(opt.outputFile, "-") == 0) {
        opt.overwrite = true;
        opt.thumb = THUMB_DISABLED;
        opt.outputFile = getPathOfStdout();
    }

    size_t outputFileLen = strlen(opt.outputFile);
    if (outputFileLen == 0)
        errx(EXIT_FAILURE, "output file cannot be empty");
    if (opt.outputFile[outputFileLen - 1] == '/')
        errx(EXIT_FAILURE, "output file cannot be a directory");

    if (opt.thumb != THUMB_DISABLED)
        opt.thumbFile = optionsNameThumbnail(opt.outputFile);

    if (note) {
        warnx("--note is deprecated. See: "
            "https://github.com/resurrecting-open-source-projects/scrot/discussions/207");
        opt.note = estrdup(note); /* TODO: investigate if dup is needed */
        scrotNoteNew(opt.note);
    }
}

static void showUsage(void)
{
    fputs("Usage:  " PACKAGE_NAME " [OPTIONS...] [FILE]\n\n"
          "A list of options with brief description is given below.\n"
          "For more detailed description, "
          "consult the "PACKAGE_NAME"(1) manpage.\n\n", stdout);
    showOptions(true);
    exit(0);
}

static void showOptions(bool human)
{
    for (size_t i = 0; i < ARRAY_COUNT(opt_description); ++i) {
        const struct option *o = &lopts[i];
        const struct option_desc *d = &opt_description[i];
        if (d->description == OPT_DEPRECATED)
            continue;

        if (human) {
            int n = 0;
            if (o->val <= UCHAR_MAX)
                n += printf("-%c, ", o->val);
            n += printf("--%s", o->name);
            if (o->has_arg == required_argument)
                n += printf(" <%s>", d->arg_description);
            else if (o->has_arg == optional_argument)
                n += printf("[=%s]", d->arg_description);
            for (; n >= 0 && n < 32; ++n)
                putchar(' ');
            printf("%s\n", d->description);
        } else {
            printf("%c\t", o->val <= UCHAR_MAX ? o->val : ' ');
            printf("%s\t", o->name);
            if (o->has_arg == required_argument)
                printf("R:%s\t", d->arg_description);
            else if (o->has_arg == optional_argument)
                printf("O:%s\t", d->arg_description);
            else
                printf("N:\t");
            printf("%s\n", d->description);
        }
    }
    exit(EXIT_SUCCESS);
}

static void showVersion(void)
{
    puts(PACKAGE_NAME " version " PACKAGE_VERSION);
    exit(0);
}

static char *optionsNameThumbnail(const char *name)
{
    const ptrdiff_t nameLength = strlen(name);
    const char thumbSuffix[] = "-thumb";
    Stream ret = {0};
    char *extension;
    size_t extLength = scrotHaveFileExtension(name, &extension);
    const ptrdiff_t baseNameLength = nameLength - extLength;

    streamMem(&ret, name, baseNameLength);
    streamMem(&ret, thumbSuffix, sizeof(thumbSuffix)-1);
    streamMem(&ret, extension, extLength);
    streamChar(&ret, '\0');

    return ret.buf;
}

void optionsParseAutoselect(char *optarg)
{
    char *token;
    int *dimensions[] = {&opt.autoselectX, &opt.autoselectY, &opt.autoselectW,
        &opt.autoselectH, NULL /* Sentinel. */};
    int i = 0;
    int min;
    const char *errmsg;

    /* Geometry dimensions must be in format x,y,w,h */
    token = strtok(optarg, ",");
    for (; token != NULL; token = strtok(NULL, ",")) {
        if (dimensions[i] == NULL)
            errx(EXIT_FAILURE, "option --autoselect: too many dimensions");

        min = i >= 2; /* X,Y offsets may be 0. Width and height may not. */
        *dimensions[i] = optionsParseNum(token, min, INT_MAX, &errmsg);
        if (errmsg) {
            errx(EXIT_FAILURE, "option --autoselect: '%s' is %s", token,
                errmsg);
        }
        i++;
    }
    if (i < 4)
        errx(EXIT_FAILURE, "option --autoselect: too few dimensions");
}

static void optionsParseThumbnail(char *optarg)
{
    char *height;
    const char *errmsg;

    if ((height = strchr(optarg, 'x')) != NULL) { /* optarg is a resolution. */
        /* optarg holds the width, height holds the height. */
        *height++ = '\0';

        opt.thumb = THUMB_RES;
        opt.thumbW = optionsParseNum(optarg, 0, INT_MAX, &errmsg);
        if (errmsg) {
            errx(EXIT_FAILURE, "option --thumb: resolution width '%s' is %s",
                optarg, errmsg);
        }

        opt.thumbH = optionsParseNum(height, 0, INT_MAX, &errmsg);
        if (errmsg) {
            errx(EXIT_FAILURE, "option --thumb: resolution height '%s' is %s",
                height, errmsg);
        }

        if (opt.thumbW == 0 && opt.thumbH == 0)
            errx(EXIT_FAILURE, "option --thumb: both width and height are 0");
    } else { /* optarg is a percentage. */
        opt.thumb = THUMB_PERCENT;
        opt.thumbPercent = optionsParseNum(optarg, 1, 100, &errmsg);
        if (errmsg) {
            errx(EXIT_FAILURE, "option --thumb: percentage '%s' is %s", optarg,
                errmsg);
        }
    }
}
