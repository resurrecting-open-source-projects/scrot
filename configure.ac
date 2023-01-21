dnl Process this file with autoconf to create configure.

AC_INIT([scrot], [1.8.1],
        [https://github.com/resurrecting-open-source-projects/scrot/issues],,
        [https://github.com/resurrecting-open-source-projects/scrot])
AC_CONFIG_SRCDIR([src/scrot.c])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([src/config.h])

# Checks for programs.
orig_CFLAGS="${CFLAGS}" # Save CFLAGS before AC_PROG_CC sets them.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

AM_MAINTAINER_MODE

m4_pattern_forbid([^AX_],[=> GNU autoconf-archive not present. <=])
AS_IF([test "x$orig_CFLAGS" = "x"], [
    CFLAGS=""
    SCROT_FLAGS="-O2 -flto -g -Wall -Wextra -Wpedantic"
    AX_APPEND_COMPILE_FLAGS(["$SCROT_FLAGS"])
    AX_APPEND_LINK_FLAGS(["$SCROT_FLAGS"])
])

# Checks for libraries.
PKG_CHECK_MODULES([X11], [x11])
PKG_CHECK_MODULES([XCOMPOSITE], [xcomposite])
PKG_CHECK_MODULES([XEXT], [xext])
PKG_CHECK_MODULES([XFIXES], [xfixes])
PKG_CHECK_MODULES([XINERAMA], [xinerama])
PKG_CHECK_MODULES([IMLIB2], [imlib2])

AC_ARG_ENABLE([libbsd-feature-test],
    AS_HELP_STRING([--enable-libbsd-feature-test],
        ["Do not configure the program, return true if libbsd is needed"]))
AC_CHECK_FUNCS([strlcpy strlcat err errx warn warnx],, [LIBBSD_NEEDED=yes])
AC_CHECK_HEADERS([sys/queue.h],, [LIBBSD_NEEDED=yes])
# libbsd is obligatory on systems that don't have the BSD functions we use.
# Pass "--enable-libbsd-feature-test" to ./configure, and the configure script
# will tell you whether the library is a dependency. This option is intended to
# be used by package maintainers.
AS_IF([test "x$enable_libbsd_feature_test" = "xyes"],
    AS_IF([test "x$LIBBSD_NEEDED" = "xyes"], [
        AC_MSG_NOTICE([scrot depends on libbsd in the current system])
	exit 0
    ], [
        AC_MSG_NOTICE([scrot does not depend on libbsd in the current system])
        exit 1
    ])
)
AS_IF([test "x$LIBBSD_NEEDED" = "xyes"], [
    PKG_CHECK_MODULES([LIBBSD], [libbsd-overlay],,
        [AC_MSG_ERROR([BSD functions not found, libbsd is required])])
])

AC_SUBST([LIBS], ["$X11_LIBS $XCOMPOSITE_LIBS $XEXT_LIBS $XFIXES_LIBS \
    $XINERAMA_LIBS $IMLIB2_LIBS $LIBBSD_LIBS $LIBS"])
AC_SUBST([CPPFLAGS], ["$X11_CFLAGS $XCOMPOSITE_CFLAGS $XEXT_CFLAGS \
    $XFIXES_CFLAGS $XINERAMA_CFLAGS $IMLIB2_CFLAGS $LIBBSD_CFLAGS $CPPFLAGS"])

# Checks for header files.
AC_CHECK_HEADERS([stdint.h sys/time.h unistd.h])

# Required: Checks for library functions.
AC_CHECK_FUNCS([getopt_long getsubopt gethostname select strdup strerror strndup strtol],,
    AC_MSG_ERROR([required functions are not present.]))

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
