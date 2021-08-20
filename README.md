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

scrot is free software under the [MIT-advertising](COPYING) license.

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

scrot requires a few projects and libraries:

- [autoconf](https://www.gnu.org/software/autoconf/autoconf.html) (build time only)
- [autoconf-archive](https://www.gnu.org/software/autoconf-archive/) (build time only)
- [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/) (build time only)
- [imlib2](https://sourceforge.net/projects/enlightenment/files/imlib2-src/)
- [libbsd](https://libbsd.freedesktop.org/wiki/) (if `./configure --without-libbsd; make` fails)
- X [(e.g. X.Org)](https://www.x.org/wiki/)
- libXcomposite [(can be found in X.Org)](https://gitlab.freedesktop.org/xorg/lib/libxcomposite)
- libXext [(can be found in X.Org)](https://gitlab.freedesktop.org/xorg/lib/libxext)
- libxfixes [(can be found in X.Org)](https://gitlab.freedesktop.org/xorg/lib/libxfixes)

From a Unix command line, building is simple:
```
$ ./autogen.sh
$ ./configure
$ make
```

Cleaning up the build is also simple:
```
$ make distclean
```

You can return to a pristine source tree before running `./configure`:
```
$ ./autogen.sh clean
```

## Installing ##

If you've built scrot from source, scrot needs to be installed from a root shell:
```
# make install
```

Debian users can install scrot from their package manager:
```
$ sudo apt install scrot
```

## Author ##

scrot was originally developed by Tom Gilbert.

Currently, source code is maintained by volunteers. Newer versions are
available at https://github.com/resurrecting-open-source-projects/scrot
