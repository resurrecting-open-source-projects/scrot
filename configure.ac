dnl Process this file with autoconf to create configure.

AC_INIT([scrot], [1.6], [https://github.com/resurrecting-open-source-projects/scrot/issues],
		[],[https://github.com/resurrecting-open-source-projects/scrot])
AC_CONFIG_SRCDIR([src/main.c])
AM_INIT_AUTOMAKE(dist-bzip2)
AC_CONFIG_HEADER([src/config.h])
AX_PREFIX_CONFIG_H([src/scrot_config.h])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_STDC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

AM_MAINTAINER_MODE

# Checks for libraries.
PKG_CHECK_MODULES([X11], [x11])
PKG_CHECK_MODULES([XCOMPOSITE], [xcomposite])
PKG_CHECK_MODULES([XEXT], [xext])
PKG_CHECK_MODULES([XINERAMA], [xinerama])
PKG_CHECK_MODULES([XFIXES], [xfixes])
PKG_CHECK_MODULES([IMLIB2], [imlib2])

AC_ARG_WITH([libbsd],
    AS_HELP_STRING([--without-libbsd], [Error when BSD functions are not found]))
AC_CHECK_FUNCS([strlcpy strlcat err errx],, [LIBBSD_NEEDED=yes])
AS_IF([test "x$LIBBSD_NEEDED" = "xyes"], [
    AS_IF([test "x$with_libbsd" = "xno"], [
        AC_MSG_ERROR([BSD functions not found and --without-libbsd was used])
    ])
    PKG_CHECK_MODULES([LIBBSD], [libbsd-overlay],,
        [AC_MSG_ERROR([BSD functions not found, libbsd is required])])
])

AC_SUBST([LIBS], ["$X11_LIBS $XCOMPOSITE_LIBS $XEXT_LIBS $XFIXES_LIBS \
    $XINERAMA_LIBS $IMLIB2_LIBS $LIBBSD_LIBS $LIBS"])
AC_SUBST([CPPFLAGS], ["$X11_CFLAGS $XCOMPOSITE_CFLAGS $XEXT_CFLAGS \
    $XFIXES_CFLAGS $XINERAMA_CFLAGS $IMLIB2_CFLAGS $LIBBSD_CFLAGS $CPPFLAGS"])

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([stdint.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_C_CONST
AC_TYPE_SIZE_T
AC_CHECK_TYPES([ptrdiff_t])

# Required: Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([getopt_long getsubopt atexit gethostname memset select strchr strdup strerror strndup strpbrk strrchr strtol],,
	AC_MSG_ERROR([required functions are not present.]))

m4_pattern_forbid([^AX_],[=> GNU autoconf-archive not present. <=])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
