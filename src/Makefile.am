# Copyright 1999-2000 Tom Gilbert <tom@linuxbrit.co.uk,
#                                   gilbertt@linuxbrit.co.uk,
#                                   scrot_sucks@linuxbrit.co.uk>
# Copyright 2016      Joao Eriberto Mota Filho <eriberto@eriberto.pro.br>
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
# A list of all the files in the current directory which can be regenerated

MAINTAINERCLEANFILES = Makefile.in

AM_LDFLAGS        = -L/usr/X11R6/lib
AM_CPPFLAGS       = -g -O3 -Wall -I/usr/X11R6/include \
$(X_CFLAGS) -I$(prefix)/include -I$(includedir) -I. \
-DPREFIX=\""$(prefix)"\" @GIBLIB_CFLAGS@
LIBOBJS = @LIBOBJS@

bin_PROGRAMS      = scrot
scrot_SOURCES       = main.c getopt.c getopt1.c getopt.h scrot.h \
options.c options.h debug.h imlib.c structs.h note.c note.h
scrot_LDADD         = -lX11 -lXfixes -lXcursor @GIBLIB_LIBS@
