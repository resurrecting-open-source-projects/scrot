dep = [dependency('x11'), dependency('xfixes'), dependency('xext'),
       dependency('xcomposite'), dependency('imlib2')]

conf_data = configuration_data()
conf_data.set_quoted('SCROT_PACKAGE', meson.project_name())
conf_data.set_quoted('SCROT_VERSION', meson.project_version())
configure_file(output : 'scrot_config.h',
               configuration : conf_data)

executable('scrot',
           'getopt.c',
           'imlib.c',
           'main.c',
           'note.c',
           'options.c',
           'utils.c',
           'scrot_selection.c',
           'selection_classic.c',
           'selection_edge.c',
           'slist.c',
           'getopt1.c',
           install : true,
           dependencies : dep)
