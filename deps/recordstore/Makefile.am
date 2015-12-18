AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS =
bin_SCRIPTS =
noinst_LIBRARIES =
noinst_PROGRAMS =
lib_LIBRARIES =
noinst_HEADERS =
TESTS =
XFAIL_TESTS = 
check_PROGRAMS =
EXTRA_PROGRAMS =
CLEANFILES = $(bin_SCRIPTS)
MAINTAINERCLEANFILES =
EXTRA_DIST =
BUILT_SOURCES =
AM_CTAGSFLAGS=--excmd=pattern -B --extra=+f --langmap=c:+.ae.hae -I pwait,pbranch
include_HEADERS = include/recordstore.hae

EXTRA_DIST += prepare.sh maint/depend-ae.sh INSTALL COPYRIGHT

AM_CPPFLAGS = -I$(srcdir)/include -I.
AM_CFLAGS =
AM_LIBS =

AM_CXXFLAGS = $(AM_CFLAGS)

lib_LIBRARIES += src/librecordstore.a
src_librecordstore_a_SOURCES = 

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = maint/recordstore.pc

AE_SRC = 
AE_HDR = 

include Make.rules

include $(top_srcdir)/src/Makefile.subdir
include $(top_srcdir)/tests/Makefile.subdir

-include $(AE_SRC:.ae=.ae.d)
-include $(AE_HDR:.hae=.hae.d)
