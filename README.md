# scrot (SCReenshOT)

#### scrot - command line screen capture utility

## Help this project ##

scrot needs your help. **If you are a programmer** and if you wants to help a
nice project, this is your opportunity.

scrot (SCReenshOT) was imported from some tarballs from Debian Project[1].
After this, all patches found in Debian project and other places for this program
were applied. All initial work was registered in ChangeLog file (version 0.9 and
later releases). scrot is packaged in Debian[2] Project.

If you are interested to help SCReenshOT, read the [CONTRIBUTING.md](CONTRIBUTING.md) file.

[1]: https://snapshot.debian.org/package/scrot/
[2]: https://tracker.debian.org/pkg/scrot

## What is scrot (SCReenshOT)? ##

scrot (SCReenshOT) is a simple command line screen capture utility that uses
imlib2 to grab and save images. Multiple image formats are supported through
imlib2's dynamic saver modules.

Some features of the scrot:
  - support to multiple image formats (JPG, PNG, GIF, etc.).
  - optimization of the screen shots image quality.
  - capture a specific window or a rectangular area on the screen with the
    help of switch.

scrot also can be used to monitor a desktop PC in admin absent and register
unwanted activities.

Nowadays, scrot is maintained by volunteers.

## Build and Install ##

scrot depends of the following libraries/artifacts to build:

  - autoconf-archive
  - giblib
  - imlib2
  - libtool
  - libxfixes

To build and install, run the following commands:

    $ ./autogen.sh
    $ ./configure
    $ make
    # make install

To return to original source code you can use '$ make distclean' command.

On Debian systems you can use '# apt install scrot'.

## Author ##

scrot was originally developed by Tom Gilbert under MIT-advertising license.

Currently, source code is maintained by volunteers. Newer versions are
available at https://github.com/resurrecting-open-source-projects/scrot
