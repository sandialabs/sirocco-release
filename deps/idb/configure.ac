#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([idb], [0.2])

AC_CANONICAL_TARGET
AC_CANONICAL_SYSTEM
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([foreign subdir-objects -Wall])

# we should remove this soon, only needed for automake 1.10 and older
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_SRCDIR([INSTALL])
AC_CONFIG_HEADERS([idb-config.h])

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

AC_CONFIG_FILES([Makefile
maint/aeidb.pc
maint/idb.pc])

AC_OUTPUT
