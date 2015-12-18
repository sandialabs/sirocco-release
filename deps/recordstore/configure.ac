#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([recordstore], [0.1],
[http://trac.mcs.anl.gov/projects/triton/newticket],[],[http://www.mcs.anl.gov/projects/triton/]) 

AC_CANONICAL_TARGET
AC_CANONICAL_SYSTEM
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([foreign subdir-objects -Wall])

# we should remove this soon, only needed for automake 1.10 and older
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_SRCDIR([INSTALL])
AC_CONFIG_HEADERS([recordstore-config.h])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_RANLIB
AC_PROG_MKDIR_P
AC_REQUIRE_CPP

AC_CHECK_SIZEOF([long int])

dnl
dnl Verify pkg-config
dnl
PKG_PROG_PKG_CONFIG
PKG_CONFIG="pkg-config --static"
PKG_CHECK_MODULES([AESOP], [aesop], [],
   [AC_MSG_ERROR([Could not find working aesop installation!])])

LIBS="$AESOP_LIBS $LIBS"
CPPFLAGS="$AESOP_CFLAGS $CPPFLAGS"
CFLAGS="$AESOP_CFLAGS $CFLAGS"

dnl Find aecc wrapper
PKG_CHECK_VAR([AECC_PREFIX],[aesop], [exec_prefix])
AECC="${AECC_PREFIX}/bin/aecc"
AC_SUBST(AECC)

dnl ====================================================
dnl Use pkgconfig to find c-utils
dnl  - Sets C_UTILS_CFLAGS, C_UTILS_LIBS
dnl ====================================================
PKG_CHECK_MODULES([C_UTILS],[c-utils], [],
   [AC_MSG_ERROR([Could not find working c-utils installation!])])
LIBS="$C_UTILS_LIBS $LIBS"
CFLAGS="$C_UTILS_CFLAGS $CFLAGS"
dnl unfortunately pkg-config doesn't separate cppflags/cflags
CPPFLAGS="$C_UTILS_CPPFLAGS $CPPFLAGS"

AC_ARG_ENABLE(strict,
[  --disable-strict	Turn off strict compiler warnings],
[],
[
    CFLAGS="-Wall -Wcast-align -Wbad-function-cast -Wmissing-prototypes -Wmissing-declarations -Wundef -Wpointer-arith -Wnested-externs -Wredundant-decls -Wextra -Wcast-qual -Wshadow -Wwrite-strings -Wformat -Wunused -Wno-unused-label -Wno-sign-compare -Wno-unused-parameter $CFLAGS"
])

# check for O_DIRECT
AC_MSG_CHECKING([for O_DIRECT])
AC_TRY_COMPILE([
#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
], [
int fd = O_DIRECT;
], 
AC_MSG_RESULT(yes)
AC_DEFINE([HAVE_ODIRECT], [], [Define if able to open files with O_DIRECT])
,
AC_MSG_RESULT(no)
AC_MSG_WARN([Unable to use O_DIRECT]))

AC_MSG_CHECKING([for O_NOATIME flag])
AC_TRY_COMPILE(
    [
     #define _GNU_SOURCE
     #include <sys/types.h>
     #include <sys/stat.h>
     #include <fcntl.h>
    ],
    [
     int flag = O_NOATIME;
    ],
    AC_MSG_RESULT(yes)
    AC_DEFINE(HAVE_O_NOATIME, 1, Define if O_NOATIME flag exists)
    ,
    AC_MSG_RESULT(no)
)

AC_MSG_CHECKING([for FALLOC_FL_PUNCH_HOLE flag])
AC_TRY_COMPILE(
    [
     #define _GNU_SOURCE
     #include <fcntl.h>
    ],
    [
     int mode = FALLOC_FL_PUNCH_HOLE;
    ],
    AC_MSG_RESULT(yes)
    AC_DEFINE(HAVE_FALLOC_FL_PUNCH_HOLE, 1, Define if FALLOC_FL_PUNCH_HOLE flag exists)
    ,
    AC_MSG_RESULT(no)
)

AC_ARG_ENABLE(kinetic,
              [--enable-kinetic Enable support for Aesop kinetic interfaces],
              [have_kinetic=yes], [have_kinetic=no])

AM_CONDITIONAL([HAVE_KINETIC], [test "x${have_kinetic}" == xyes])
AM_COND_IF([HAVE_KINETIC],
           AC_DEFINE([HAVE_KINETIC], 1, [aesop kinetic support enabled]))

AC_CONFIG_FILES([Makefile
maint/recordstore.pc])

AC_OUTPUT
