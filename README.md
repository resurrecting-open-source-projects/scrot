![Logo](doc/scrot.png)

# scrot (SCReenshOT)

#### scrot - command line screen capture utility

## What is scrot? ##

scrot is a simple command line screen capture utility, it uses imlib2 to grab
and save images.

scrot has many useful features:
  - Support for multiple image formats: JPG, PNG, GIF, and others.
  - The screenshot's quality is configurable.
  - It is possible to capture a specific window or a rectangular area on the
    screen.

Because scrot is a command line utility, it can easily be scripted and put to
novel uses. For instance, scrot can be used to monitor an X server in absence.

scrot is free software under the [MIT-feh](COPYING) license.

## Help this project ##

scrot needs your help. **If you are a programmer** and want to help a nice
project, this is your opportunity.

The original scrot went unmaintained; the source of the last version, 0.8, was
[imported from Debian](https://snapshot.debian.org/package/scrot/). After,
patches from Debian and elsewhere were applied to create the 0.9 release. The
details of our releases are registered in the [ChangeLog](ChangeLog) file.
Now, scrot is maintained by volunteers under [Resurrecting Open Source Projects](https://github.com/resurrecting-open-source-projects).

If you are interested in helping scrot, read the [CONTRIBUTING.md](CONTRIBUTING.md) file.

## Building ##

This section describes the steps to build and install scrot.

### Dependencies ###

scrot requires a few projects and libraries:

- [autoconf](https://www.gnu.org/software/autoconf/autoconf.html) (build time only)
- [autoconf-archive](https://www.gnu.org/software/autoconf-archive/) (build time only)
- A pkg-config implementation [(e.g. pkgconf)](https://github.com/pkgconf/pkgconf) (build time only)
- [imlib2](https://sourceforge.net/projects/enlightenment/files/imlib2-src/)
  (must be built with X, text and filters support)
- [libbsd](https://libbsd.freedesktop.org/wiki/) (only needed if `<err.h>` is missing)
- An X11 implementation [(e.g. X.Org)](https://www.x.org/wiki/)
- libXcomposite [(can be found in X.Org)](https://gitlab.freedesktop.org/xorg/lib/libxcomposite)
- libXext [(can be found in X.Org)](https://gitlab.freedesktop.org/xorg/lib/libxext)
- libXfixes [(can be found in X.Org)](https://gitlab.freedesktop.org/xorg/lib/libxfixes)
- libXinerama [(can be found in X.Org)](https://gitlab.freedesktop.org/xorg/lib/libxinerama)

### Generic installation instructions ###

If you are building from a git checkout, or if you have applied additional
patches to a tarball release, run:
```console
$ ./autogen.sh
```
TIP: if the source code does not have a _configure_ file, you need to run
`./autogen.sh`.

If you have a tarball release, or after running the command above on a git
checkout or a patched tarball, run:
```console
$ ./configure && make
```

To install the compiled result, run as root:
```console
# make install
```
Or, as a regular user:
```console
$ sudo make install
```

Cleaning up the build is also simple:
```console
$ make clean
```

To vanish the source code, removing all pre-built files, including _configure_,
run:
```console
$ make distclean
```

You can return to a pristine source tree before running `./configure`:
```console
$ ./autogen.sh clean
```

Debian users can install scrot from their package manager:
```console
$ sudo apt install scrot
```

Bash and Zsh completion scripts are available in [etc/](./etc).
Distro packagers are encouraged to install them to the appropriate directory.

## Author ##

scrot was originally developed by Tom Gilbert.

Currently, source code is maintained by volunteers. Newer versions are
available at https://github.com/resurrecting-open-source-projects/scrot
