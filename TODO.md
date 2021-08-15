# TODO

#### The lists in this page summarize current efforts in the scrot project.

## Fix bugs

Look for any bugs reported downstream or in our github issues and fix them:
- https://github.com/resurrecting-open-source-projects/scrot/issues
- https://bugs.debian.org/cgi-bin/pkgreport.cgi?dist=unstable;package=scrot
- https://bugs.launchpad.net/ubuntu/+source/scrot/+bugs
- https://apps.fedoraproject.org/packages/scrot/bugs
- https://bugs.gentoo.org/buglist.cgi?quicksearch=scrot

## Integrate [libbsd](https://libbsd.freedesktop.org/wiki/)

The BSD systems provide nonstandard C functions; many of the functions remove
the need for some boilerplate code, and provide safer or easier to use APIs
compared to the C standard library.
scrot is in the process of auditing its source to make use of BSD extensions
to the C library where they are found to improve code quality. scrot depends on
libbsd to provide the required functions where they're missing to remain
portable. libbsd includes manuals for all the functions it implements-a list of
them can be found at libbsd(7). The manual can alternatively be accessed from
the web at [manpages.debian.org](https://manpages.debian.org/unstable/libbsd-dev/libbsd.7.en.html).
BSD systems provide manuals for these functions in the default install.

The following libbsd integration efforts are in progress:
- Replace strncpy() and strcpy() calls with strlcpy().
- Replace strcat() and strncat() calls with strlcat().
- ~~Replace error exits with the err() family of functions.~~
- ~~Replace local implementations of data structures with the sys/queue.h API.~~
- Find places where other BSD functions can serve scrot well.

## Comply with WebKit style
We're currently in the process of complying with the new obligatory C style
described in [CONTRIBUTING.md](CONTRIBUTING.md).

The following files are yet to be converted:
- ~~src/imlib.c~~
- ~~src/main.c~~
- ~~src/note.c~~
- ~~src/options.c~~
- ~~src/scrot_selection.c~~
- ~~src/selection_classic.c~~
- ~~src/selection_edge.c~~
- ~~src/note.h~~
- ~~src/options.h~~
- ~~src/slist.c~~
- ~~src/scrot.h~~
- ~~src/scrot_selection.h~~
- ~~src/selection_classic.h~~
- ~~src/selection_edge.h~~
- ~~src/slist.h~~
- ~~src/structs.h~~
