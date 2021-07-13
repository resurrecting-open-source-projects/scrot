# Copyright (C) 2021 Guilherme Janczak
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

dep = [dependency('x11'), dependency('xfixes'), dependency('xext'),
       dependency('xcomposite'), dependency('imlib2')]

conf_data = configuration_data()
conf_data.set_quoted('SCROT_PACKAGE', meson.project_name())
conf_data.set_quoted('SCROT_VERSION', meson.project_version())
configure_file(output : 'scrot_config.h',
               configuration : conf_data)

executable('scrot',
           'imlib.c',
           'main.c',
           'note.c',
           'options.c',
           'scrot_selection.c',
           'selection_classic.c',
           'selection_edge.c',
           'slist.c',
           install : true,
           dependencies : dep)
