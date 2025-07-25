dnl Process this file with autoconf to create configure.

AC_INIT([scrot], [1.12.1],
        [https://github.com/resurrecting-open-source-projects/scrot/issues],,
        [https://github.com/resurrecting-open-source-projects/scrot])
AC_CONFIG_SRCDIR([src/scrot.c])
AM_INIT_AUTOMAKE

# Checks for programs.
orig_CFLAGS="${CFLAGS}" # Save CFLAGS before AC_PROG_CC sets them.
AC_PROG_CC
# In autoconf < 2.70, AC_PROG_CC determines the compiler, and AC_PROG_CC_STDC
# sets it to the newest C standard supported by autoconf.
# In autoconf >= 2.70, AC_PROG_CC determines the compiler and sets it to the
# newest C standard supported by autoconf, and AC_PROG_CC_STDC is deprecated.
m4_version_prereq([2.70],, [AC_PROG_CC_STDC])
AC_PROG_INSTALL
AC_PROG_MAKE_SET

AM_MAINTAINER_MODE

# Install bash completion script
AC_ARG_WITH([bash-completion-path],
    [AS_HELP_STRING([--with-bash-completion-path=DIR],
        [Custom installation directory for the bash completion script (Default=/usr/share/bash-completion/completions)])],
    [bash_completion_dir="$withval"],
    [bash_completion_dir="/usr/share/bash-completion/completions"])

# Install zsh completion script
AC_ARG_WITH([zsh-completion-path],
    [AS_HELP_STRING([--with-zsh-completion-path=DIR],
        [Custom installation directory for the zsh completion script (Default=/usr/share/zsh/site-functions)])],
    [zsh_completion_dir="$withval"],
    [zsh_completion_dir="/usr/share/zsh/site-functions"])

m4_pattern_forbid([^AX_],[=> GNU autoconf-archive not present. <=])
AS_IF([test "x$orig_CFLAGS" = "x"], [
    CFLAGS=""
    AX_APPEND_COMPILE_FLAGS(["-flto"])
    AS_IF([test "x$CFLAGS" = "x-flto"], [
        LTO_ENABLED=yes
        AX_APPEND_LINK_FLAGS(["-flto"])
    ])
    m4_foreach([SCROT_FLAG],
        [["-O3"], ["-Wall"], ["-Wextra"], ["-Wpedantic"]], [
            AX_APPEND_COMPILE_FLAGS(["SCROT_FLAG"])
            AS_IF([test "x$LTO_ENABLED" = "xyes"], [
                AX_APPEND_LINK_FLAGS(["SCROT_FLAG"])
            ])
        ]
    )
    # SCROT_PRIVATE_FLAGS are mainly used by the CI to append additional flags.
    # append them unconditionally, the CI shouldn't try to add flags the
    # compiler doesn't support.
    AC_SUBST([CFLAGS], ["$CFLAGS $SCROT_PRIVATE_FLAGS"])
    AS_IF([test "x$LTO_ENABLED" = "xyes"], [
        AC_SUBST([LDFLAGS], ["$LDFLAGS $SCROT_PRIVATE_FLAGS"])
    ])
])

# Checks for libraries.
m4_ifndef([PKG_PREREQ],
     [m4_fatal([either pkg-config or pkg.m4 is not installed])])
PKG_CHECK_MODULES([SCROT_DEPS], ["$srcdir/deps.pc"])

# TODO: When -Wpedantic and -Werror are both set:
# - header checks fail due to empty translation unit:
#   https://github.com/resurrecting-open-source-projects/scrot/pull/333#issuecomment-1572157050
# - functions checks fail due to -Wstrict-prototypes:
#   https://github.com/resurrecting-open-source-projects/scrot/pull/356#issuecomment-1608585396
# figure out a way to fix these and remove `-Wno-error=pedantic` from the CI.
AC_CHECK_FUNCS([err errx warn warnx],, [LIBBSD_NEEDED=yes])
AS_IF([test "x$LIBBSD_NEEDED" = "xyes"], [
    PKG_CHECK_MODULES([LIBBSD], [libbsd-overlay],,
        [AC_MSG_ERROR([BSD functions not found, libbsd is required])])
])

AM_CONDITIONAL(BASH, [test "x$bash_completion_dir" != "xno"])
AM_CONDITIONAL(ZSH, [test "x$zsh_completion_dir" != "xno"])

AC_SUBST([LIBS], ["$SCROT_DEPS_LIBS $LIBBSD_LIBS $LIBS"])
AC_SUBST([CPPFLAGS], ["$SCROT_DEPS_CFLAGS $LIBBSD_CFLAGS $CPPFLAGS"])

AC_SUBST([bash_completion_dir])
AC_SUBST([zsh_completion_dir])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
