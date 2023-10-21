#!/bin/sh

# Copyright 2021 Guilherme Janczak <guilherme.janczak@yandex.com>
# Copyright 2023 NRK <nrk@disroot.org>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies of the Software and its documentation and acknowledgment shall be
# given in the documentation and software packages that this Software was
# used.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


if [ "$#" -eq 0 ]; then
    autoreconf -i || { echo "autoreconf failed." >&2; exit 1; }
    echo "autogen.sh: done."
    echo "Run './configure && make && make install' to install."
    echo "Or run './autogen.sh clean' to clean up."
elif [ "$#" -eq 1 ] && [ "$1" = "clean" ]; then
    if [ -e Makefile ]; then
        echo "Cannot clean. Run 'make distclean' first." >&2
        exit 1
    else
        echo "Cleaning up."
        rm -rf aclocal.m4 autom4te.cache/ compile configure depcomp install-sh \
               Makefile.in missing src/config.h.in src/Makefile.in
    fi
else
    printf 'Invalid command: %s\n' "$*" >&2
    exit 1
fi
