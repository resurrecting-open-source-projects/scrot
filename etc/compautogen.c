#include <limits.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

enum { no_argument, required_argument, optional_argument, };
struct option {
    const char *name;
    int has_arg;
    int *flag;
    int val;
    const char *desc;
};

#define DESC(X) X
#include "../src/opts.h"
enum { lopts_len = (sizeof(lopts) / sizeof(lopts[0])) - 1 };

static bool isDeprecated(const struct option *o)
{
    return !strcmp(o->name, "note") || !strcmp(o->name, "script") ||
        !strcmp(o->name, "focussed"); /* not deprecated, but no point in a double entry */
}

static int zsh(FILE *f)
{
    fputs("#compdef scrot\n\n", f);
    fputs("_arguments \\\n", f);
    for (const struct option *o = lopts, *end = o + lopts_len; o < end; ++o) {
        if (isDeprecated(o))
            continue;

        fprintf(f, "\t'--%s", o->name);
        if (o->desc)
            fprintf(f, "[%s]", o->desc);
        fprintf(f, "' \\\n");

        if (o->val <= UCHAR_MAX) {
            fprintf(f, "\t'-%c", o->val);
            if (o->desc)
                fprintf(f, "[%s]", o->desc);
            fprintf(f, "' \\\n");
        }
    }
    fputc('\n', f);
    fflush(f);
    return ferror(f);
}

static int bash(FILE *f)
{
    fputs("complete -W '", f);
    for (const struct option *o = lopts, *end = o + lopts_len; o < end; ++o) {
        if (isDeprecated(o))
            continue;

        fprintf(f, "--%s ", o->name);
        if (o->val <= UCHAR_MAX)
            fprintf(f, "-%c ", o->val);
    }
    fputs("' scrot\n", f);
    fflush(f);
    return ferror(f);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: compautogen <zsh|bash>\n");
        return 1;
    }

    char *s = argv[1];
    if (!strcmp(s, "zsh"))
        return zsh(stdout);
    else if (!strcmp(s, "bash"))
        return bash(stdout);

    fprintf(stderr, "Unknown command\n");
    return 1;
}
