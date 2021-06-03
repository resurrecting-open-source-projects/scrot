dnl Process this file with autoconf to create configure.

AC_INIT([scrot], [1.5], [https://github.com/resurrecting-open-source-projects/scrot/issues])
AC_CONFIG_SRCDIR([src/main.c])
AM_INIT_AUTOMAKE(dist-bzip2)
AC_CONFIG_HEADER([src/config.h])
AX_PREFIX_CONFIG_H([src/scrot_config.h])

AC_PROG_CC
AM_PROG_CC_STDC
AC_C_CONST

AC_PROG_INSTALL
AC_PROG_MAKE_SET

AM_MAINTAINER_MODE

AC_PATH_GENERIC(Imlib2, , [
  AC_SUBST(IMLIB2_LIBS)
  AC_SUBST(IMLIB2_CFLAGS) ],
  AC_MSG_ERROR(Cannot find Imlib2: Is imlib2-config in the path?) )


LIBS="$LIBS -lm"
IMLIB2_LIBS=`imlib2-config --libs`
IMLIB2_CFLAGS=`imlib2-config --cflags`
AC_SUBST(IMLIB2_LIBS)
AC_SUBST(IMLIB2_CFLAGS)

AC_CHECK_FUNC(getopt_long,,[LIBOBJS="$LIBOBJS getopt.o getopt1.o"])
AC_SUBST(LIBOBJS)
AC_CHECK_FUNC(strdup,,)

m4_pattern_forbid([^AX_],[=> GNU autoconf-archive not present. <=])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
