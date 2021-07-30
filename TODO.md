# TODO

#### The bullet lists in this page include describe efforts in the scrot project.

## Integrate [libbsd](https://libbsd.freedesktop.org/wiki/) ##

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
- Replace error exits with the err() family of functions.
- Replace local implementations of data structures with the sys/queue.h API.
- Find places where other BSD functions can serve scrot well.
