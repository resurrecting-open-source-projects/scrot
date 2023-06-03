#ifndef H_OPTS
#define H_OPTS

enum { /* long opt only */
    /* ensure these don't collude with single byte opts. */
    OPT_FORMAT = UCHAR_MAX + 1,
};
static const char stropts[] = "a:bC:cD:d:e:F:fhik::l:M:mn:opq:S:s::t:uvw:Z:z";
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
    {0}
};

#endif
