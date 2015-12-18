#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([libssm_ref], [0.6.6-r2263], [mfarmer2@cis.uab.edu])
AM_INIT_AUTOMAKE([tar-ustar -Wall -Werror foreign])
AM_SILENT_RULES([yes])
AC_CONFIG_SRCDIR([src/ssm_put.c])
AC_CONFIG_HEADERS([config.h])

AC_PROG_RANLIB
# Checks for programs.
AC_PROG_CC
# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([memory.h netinet/in.h stddef.h stdint.h stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gettimeofday memmove memset])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
