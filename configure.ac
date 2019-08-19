dnl Process this file with autoconf to create configure.

AC_INIT([scrot], [1.2], [https://github.com/resurrecting-open-source-projects/scrot/issues])
AC_CONFIG_SRCDIR([src/main.c])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADER([src/config.h])
AX_PREFIX_CONFIG_H([src/scrot_config.h])

AC_PROG_CC
AM_PROG_CC_STDC
AC_C_CONST

AC_PROG_INSTALL
AC_PROG_MAKE_SET

AM_MAINTAINER_MODE

AC_PATH_GENERIC(giblib, 1.2.3, [
  AC_SUBST(GIBLIB_LIBS)
  AC_SUBST(GIBLIB_CFLAGS) ],
  AC_MSG_ERROR(Cannot find giblib: Is giblib-config in the path?) )

LIBS="$LIBS -lm"
GIBLIB_LIBS=`giblib-config --libs`
GIBLIB_CFLAGS=`giblib-config --cflags`
AC_SUBST(GIBLIB_LIBS)
AC_SUBST(GIBLIB_CFLAGS)

AC_CHECK_FUNC(getopt_long,,[LIBOBJS="$LIBOBJS getopt.o getopt1.o"])
AC_SUBST(LIBOBJS)

m4_pattern_forbid([^AX_],[=> GNU autoconf-archive not present. <=])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
