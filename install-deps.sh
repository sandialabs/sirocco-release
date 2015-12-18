#
# Builds standalone software that is bundled in this release. This
# currently includes:
# - SSM
# - Recordstore
# - IDB
# - HOSS
#
# These will be installed in ../include, ../bin, and ../lib.
#
# Software in this directory depends on the Aesop compiler. If this is
# not available, run ../install-aesop.sh first.
#

set -e
set -u

PFX=${PWD}
BASE=${PWD}/deps

set +u
CPPFLAGS=${CPPFLAGS}
CFLAGS=${CFLAGS}
LDFLAGS=${LDFLAGS}
PATH=${PATH}
set -u

#############################################################################
# RECORDSTORE
#############################################################################
cd ${BASE}/recordstore
./prepare.sh
./configure --prefix=${PFX} PKG_CONFIG_PATH=${PFX}/lib/pkgconfig
make install

#############################################################################
# SSM
#############################################################################
cd ${BASE}/libssm_ref-0.6.6-r2263
./configure --prefix=${PFX}
make install

cd ${BASE}/libssmptcp_ref-0.6.6-r2264
./configure --prefix=${PFX} LDFLAGS=-L${PFX}/lib CPPFLAGS=-I${PFX}/include
make install

#############################################################################
# IDB
#############################################################################
cd ${BASE}/idb
./prepare.sh
./configure --prefix=${PFX}				\
	    CPPFLAGS="${CPPFLAGS} -I${PFX}/include"	\
	    CFLAGS="${CFLAGS}"				\
	    LDFLAGS="${LDFLAGS} -L${PFX}/lib"		\
	    PKG_CONFIG_PATH="${PFX}/lib/pkgconfig"	\
	    PATH="${PATH} ${PFX}/bin"
make install

#############################################################################
# HOSS
#############################################################################i
cd ${BASE}/hoss
./prepare.sh
./configure --prefix=${PFX}				\
	    CPPFLAGS="${CPPFLAGS} -I${PFX}/include"	\
	    CFLAGS="${CFLAGS}"				\
	    LDFLAGS="${LDFLAGS} -L${PFX}/lib"		\
	    PKG_CONFIG_PATH="${PFX}/lib/pkgconfig"
make install
