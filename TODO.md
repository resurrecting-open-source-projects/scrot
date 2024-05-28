# TODO

#### The lists in this page summarize current efforts in the scrot project.

## Fix bugs

Look for any bugs reported downstream or in our github issues and fix them:
- <https://github.com/resurrecting-open-source-projects/scrot/issues>
- <https://bugs.debian.org/cgi-bin/pkgreport.cgi?dist=unstable;package=scrot>
- <https://bugs.launchpad.net/ubuntu/+source/scrot/+bugs>
- <https://bugzilla.redhat.com/buglist.cgi?bug_status=NEW&bug_status=ASSIGNED&bug_status=REOPENED&component=scrot&product=Fedora>
- <https://bugs.gentoo.org/buglist.cgi?quicksearch=scrot>

## Shell completion improvements

- Improve the bash completion script. Currently it's quite rudimentary.
- Filter out mutually exclusive options in zsh completion. E.g if `-b` is
  already provided then `--border` should no longer be considered a potential
  match.
- Add some way to install zsh/bash completion scripts. Since the directory where
  completion script go into can vary, we want the user to explicitly set the
  installation directory. E.g via the configure script
  `./configure --with-zsh-completion-path=...`.

## Switch to newer Imlib2 interfaces

These will require a minimum version bump on imlib2 and so has to be done with
care.

- Switch to using `imlib_save_image_fd()` for saving images.
- ~~Switch to `imlib_get_error()` to retrieve the cause of error when saving and
  loading images (and `imlib_strerror()` to stringify them).~~

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
- ~~Replace strncpy() and strcpy() calls with strlcpy().~~
- ~~Replace strcat() and strncat() calls with strlcat().~~
- ~~Replace error exits with the err() family of functions.~~
- ~~Replace local implementations of data structures with the sys/queue.h API.~~
- Find places where other BSD functions can serve scrot well.
