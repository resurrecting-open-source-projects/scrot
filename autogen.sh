#!/bin/sh

# autogen.sh with clean option, v0.1-scrot
# Copyright 2019 Joao Eriberto Mota Filho <eriberto@eriberto.pro.br>
#
# This file is under BSD-3-Clause license.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the authors nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

[ "$#" -ge 1 ] && {
    case "$1" in
        (clean)
            if [ -e Makefile ]; then
                echo "I can not clean. Use 'make distclean'." >&2
                exit 1
            else
                echo "Vanishing the code"
                rm -rf aclocal.m4 autom4te.cache/ compile configure depcomp install-sh \
                       Makefile.in missing src/config.h.in src/Makefile.in
                exit
            fi
        ;;
        (*)
            echo "Invalid option '$1', maybe try running './${0##*/} clean'." >&2

            exit 1
        ;;
    esac
}

# Do autoreconf
autoreconf -i && cat << MESSAGE

Done. You can use the 'clean' option to vanish the source code.
Usage example: './autogen.sh clean'

Now run './configure', 'make', and 'make install'.
MESSAGE
