# Copyright 1999-2000 Tom Gilbert <tom@linuxbrit.co.uk,
#                                   gilbertt@linuxbrit.co.uk,
#                                   scrot_sucks@linuxbrit.co.uk>
# Copyright 2015-2023 Joao Eriberto Mota Filho <eriberto@eriberto.pro.br>
# Copyright 2019-2021 Daniel T. Borelli <danieltborelli@gmail.com>
# Copyright 2021      Guilherme Janczak <guilherme.janczak@yandex.com>
# Copyright 2022      Sam James <sam@cmpct.info>
# Copyright 2023-2024 NRK <nrk@disroot.org>
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


## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 1.10 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in

man_MANS = man/scrot.1

dist_doc_DATA = AUTHORS ChangeLog CONTRIBUTING.md doc/scrot.png FAQ.md README.md TODO.md

EXTRA_DIST = $(man_MANS) autogen.sh deps.pc etc

SUBDIRS = src

if BASH
bashcompletiondir = @bash_completion_dir@
bashcompletion_DATA = etc/bash-completion/scrot
endif

if ZSH
zshcompletiondir = @zsh_completion_dir@
zshcompletion_DATA = etc/zsh-completion/_scrot
endif

distclean-local:
	-rm -rf autom4te.cache/

	-rm -f *~ \
	src/*~ \
	aclocal.m4 \
	compile \
	configure \
	depcomp \
	install-sh \
	Makefile.in \
	missing \
	src/config.h.in \
	src/Makefile.in
