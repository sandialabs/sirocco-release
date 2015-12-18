# This Makefile is intended to build things that are distributed for
# this project. This includes:
#
# * Tarballs from websites
# * Remote git repos
# * Frozen versions of externally developed programs included here
# * Portions of this project that are developed for external
#   consumption
#
# The main idea is that these are fairly static pieces of software
# that are to be updated carefully, and do not require rebuilding very
# often.

AECC_NAME=aesop-0.2.1
AEBP_NAME=ae-blocking-parser-linux-x86_64-0.2
LIBCUTILS_NAME=c-utils-0.1
OPENPA_NAME=openpa-1.0.4

PKG="pkg"
PFX=$(PWD)/install


.PHONY: all clean pkg

all: install/bin/aecc install/lib/librecordstore.a install/lib/libssmp_tcp.a install/lib/libhoss.a

pkg: pkg/$(LIBCUTILS_NAME).tar.gz pkg/$(OPENPA_NAME).tar.gz pkg/$(AEBP_NAME).tar.gz pkg/$(AECC_NAME).tar.gz

#############################################################################
# C-UTILS
#############################################################################

install/lib/libc-utils.a: pkg/$(LIBCUTILS_NAME)
	cd $< && ./configure --prefix=$(PFX) && make install

pkg/$(LIBCUTILS_NAME): pkg/$(LIBCUTILS_NAME).tar.gz
	cd pkg && tar xzf ../$<
	touch -c $@

pkg/$(LIBCUTILS_NAME).tar.gz:
	mkdir -p pkg
	wget ftp://ftp.mcs.anl.gov/pub/aesop/download/$(LIBCUTILS_NAME).tar.gz -O $@

#############################################################################
# OPENPA
#############################################################################

install/lib/libopa.a: pkg/$(OPENPA_NAME)
	cd $< && ./configure --prefix=$(PFX) && make install

pkg/$(OPENPA_NAME): pkg/$(OPENPA_NAME).tar.gz
	cd pkg && tar xzf ../$<
	touch -c $@

pkg/$(OPENPA_NAME).tar.gz:
	mkdir -p pkg
	wget http://trac.mpich.org/projects/openpa/raw-attachment/wiki/Downloads/$(OPENPA_NAME).tar.gz -O $@

#############################################################################
# AE-BLOCKING-PARSER
#############################################################################

install/bin/ae-blocking-parser: pkg/$(AEBP_NAME)
	cd $< && ./configure --prefix=$(PFX) && make install

pkg/$(AEBP_NAME): pkg/$(AEBP_NAME).tar.gz
	cd pkg && tar xzf ../$<
	touch -c $@

pkg/$(AEBP_NAME).tar.gz:
	mkdir -p pkg
	wget ftp://ftp.mcs.anl.gov/pub/aesop/download/$(AEBP_NAME).tar.gz -O $@

#############################################################################
# AE-BLOCKING-PARSER
#############################################################################

install/bin/aecc: pkg/$(AECC_NAME) install/lib/libopa.a install/bin/ae-blocking-parser install/lib/libc-utils.a
	cd $< && ./configure --prefix=$(PFX) \
	PKG_CONFIG_PATH=$(PFX)/lib/pkgconfig \
	AE_BLOCKING_PARSER=$(PFX)/bin/ae-blocking-parser && make install

pkg/$(AECC_NAME): pkg/$(AECC_NAME).tar.gz
	cd pkg && tar xzf ../$<
	touch -c $@

pkg/$(AECC_NAME).tar.gz:
	mkdir -p pkg
	wget ftp://ftp.mcs.anl.gov/pub/aesop/download/$(AECC_NAME).tar.gz -O $@

#############################################################################
# RECORDSTORE
#############################################################################

install/lib/librecordstore.a: install/bin/aecc
	cd recordstore; \
	./prepare.sh; \
	./configure --prefix=$(PFX) PKG_CONFIG_PATH=$(PFX)/lib/pkgconfig; \
	make install

#############################################################################
# SSM
#############################################################################

install/lib/libssm.a:
	cd libssm_ref-0.6.6-r2263 && ./configure --prefix=$(PFX) && make install

install/lib/libssmp_tcp.a: install/lib/libssm.a
	cd libssmptcp_ref-0.6.6-r2264 && ./configure --prefix=$(PFX) LDFLAGS=-L$(PFX)/lib CPPFLAGS=-I$(PFX)/include && make install

#############################################################################
# HOSS
#############################################################################i
install/lib/libhoss.a: install/lib/libidb.a
	cd hoss;					\
	./prepare.sh; 					\
	./configure --prefix=$(PFX) 			\
		CPPFLAGS="$(CPPFLAGS) -I$(PFX)/include"	\
		CFLAGS="$(CFLAGS)"			\
		LDFLAGS="$(LDFLAGS) -L$(PFX)/lib"	\
		PKG_CONFIG_PATH="$(PFX)/lib/pkgconfig"; \
	make install

#############################################################################
# IDB
#############################################################################
install/lib/libidb.a:
	cd idb; 					\
	./prepare.sh; 					\
	./configure --prefix=$(PFX)			\
		CPPFLAGS="$(CPPFLAGS) -I$(PFX)/include"	\
		CFLAGS="$(CFLAGS)"			\
		LDFLAGS="-L$(PFX)/lib"			\
		PKG_CONFIG_PATH="$(PFX)/lib/pkgconfig"	\
		PATH="$(PATH) $(PFX)/bin"; 		\
	make install
