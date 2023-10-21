#ifndef H_OPTS
#define H_OPTS

#ifndef DESC
    #define DESC(X)
#endif

enum { /* long opt only */
    /* ensure these don't collude with single byte opts. */
    OPT_FORMAT = UCHAR_MAX + 1,
};
static const char stropts[] = "a:bC:cD:d:e:F:fhik::l:M:mn:opq:S:s::t:uvw:Z:z";
static const struct option lopts[] = {
    {
        "autoselect",      required_argument,  NULL,   'a',
        DESC("autoselect region x,y,w,h")
    },
    {
        "border",          no_argument,        NULL,   'b',
        DESC("also grab the window border")
    },
    {"class",           required_argument,  NULL,   'C'},
    {
        "count",           no_argument,        NULL,   'c',
        DESC("display a countdown for the delay")
    },
    {"display",         required_argument,  NULL,   'D'},
    {
        "delay",           required_argument,  NULL,   'd',
        DESC("specify a delay before screenshotting")
    },
    {"exec",            required_argument,  NULL,   'e'},
    {
        "file",            required_argument,  NULL,   'F',
        DESC("specify the output file")
    },
    {"freeze",          no_argument,        NULL,   'f'},
    {"help",            no_argument,        NULL,   'h'},
    {
        "ignorekeyboard",  no_argument,        NULL,   'i',
        DESC("do not exit on keypress")
    },
    {"stack",           optional_argument,  NULL,   'k'},
    {"line",            required_argument,  NULL,   'l'},
    {"monitor",         required_argument,  NULL,   'M'},
    {"multidisp",       no_argument,        NULL,   'm'},
    {"note",            required_argument,  NULL,   'n'},
    {
        "overwrite",       no_argument,        NULL,   'o',
        DESC("overwrite the output file if needed")
    },
    {
        "pointer",         no_argument,        NULL,   'p',
        DESC("capture the mouse pointer also")
    },
    {
        "quality",         required_argument,  NULL,   'q',
        DESC("specify output file quality")
    },
    {"script",          required_argument,  NULL,   'S'},
    {
        "select",          optional_argument,  NULL,   's',
        DESC("interactively select a region to capture")
    },
    {
        "thumb",           required_argument,  NULL,   't',
        DESC("also generate a thumbnail")
    },
    {
        "focused",         no_argument,        NULL,   'u',
        DESC("use currently focused window")
    },
    /* macquarie dictionary has both spellings */
    {"focussed",        no_argument,        NULL,   'u'},
    {"version",         no_argument,        NULL,   'v'},
    {
        "window",          required_argument,  NULL,   'w',
        DESC("specify a window ID to capture")
    },
    {
        "compression",     required_argument,  NULL,   'Z',
        DESC("output file compression setting")
    },
    {
        "silent",          no_argument,        NULL,   'z',
        DESC("prevent beeping")
    },
    {
        "format",          required_argument,  NULL, OPT_FORMAT,
        DESC("specify the output format")
    },
    {0}
};

#endif
