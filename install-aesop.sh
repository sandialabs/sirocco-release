#
# Helper script to install Aesop. If you wish to only localize files
# without building, use the argument --no-build.
#

set -e
set -u
build=1

# Process some arguments
TEMP=`getopt -o n --long no-build -n 'install-aesop.sh' -- "$@"`
if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi
eval set -- "$TEMP"
set +u
while true ; do
    case "$1" in
	-n | --no-build) build=0; echo "Will not extract/build" ; shift ;;
	--) shift ; break ;;
	*) echo "Internal error!" ; exit 1 ;;
    esac
done
if [ "$1" != "" ]; then
    echo "Unrecognized argument: $1"
    exit 1
fi
set -u

# Helper function - If argument 1 doesn't exist, a download from
# argument 2 is performed.
download_if_absent()
{
    set +u
    FILENAME=$1
    URL=$2
    set -u

    if [ -z $FILENAME ] || [ -z $URL ]; then
	echo Internal error, download_if_absent FILENAME = $FILENAME URL = $URL
	exit 1
    fi
    if [ ! -f ${FILENAME} ]; then
	echo "${PWD}/${FILENAME} does not exist, downloading from $URL"
	wget ${URL}
    else
	echo "${PWD}/${FILENAME} exists, will not download"
    fi
}

# Structure
AEPKG=${PWD}/aesop-pkgs
PFX=${PWD}
mkdir -p ${AEPKG}

# Download and install libev
cd ${AEPKG}
LIBEV_NAME=libev-4.22
download_if_absent ${LIBEV_NAME}.tar.gz http://dist.schmorp.de/libev/libev-4.22.tar.gz
if [ "${build}" = "1" ]; then
    tar xf ${LIBEV_NAME}.tar.gz
    cd ${LIBEV_NAME}
    ./configure --prefix=${PFX}
    make install
fi

# Download and install libopa
cd ${AEPKG}
OPENPA_NAME=openpa-1.0.4
download_if_absent ${OPENPA_NAME}.tar.gz http://trac.mpich.org/projects/openpa/raw-attachment/wiki/Downloads/${OPENPA_NAME}.tar.gz
if [ "${build}" = "1" ]; then
    tar xf ${OPENPA_NAME}.tar.gz
    cd ${OPENPA_NAME}
    ./configure --prefix=${PFX}
    make install
fi

# Download and install libcutils
LIBCUTILS_NAME=c-utils-0.1
cd ${AEPKG}
download_if_absent ${LIBCUTILS_NAME}.tar.gz ftp://ftp.mcs.anl.gov/pub/aesop/download/${LIBCUTILS_NAME}.tar.gz
if [ "${build}" = "1" ]; then
    tar xf ${LIBCUTILS_NAME}.tar.gz
    cd ${LIBCUTILS_NAME}
    ./configure --prefix=${PFX}
    make install
fi

# Download and install aesop-blocking-parser
cd ${AEPKG}
AEBP_NAME=ae-blocking-parser-linux-x86_64-0.2
download_if_absent ${AEBP_NAME}.tar.gz ftp://ftp.mcs.anl.gov/pub/aesop/download/${AEBP_NAME}.tar.gz
if [ "${build}" = "1" ]; then
    tar xf ${AEBP_NAME}.tar.gz
    cd ${AEBP_NAME}
    ./configure --prefix=${PFX}
    make install
fi

# Download and install aesop
AECC_NAME=aesop-0.2.1
cd ${AEPKG}
download_if_absent ${AECC_NAME}.tar.gz ftp://ftp.mcs.anl.gov/pub/aesop/download/${AECC_NAME}.tar.gz
if [ "${build}" = "1" ]; then
    tar xf ${AECC_NAME}.tar.gz
    cd ${AECC_NAME}
    ./configure --prefix=${PFX} \
		PKG_CONFIG_PATH=${PFX}/lib/pkgconfig \
		AE_BLOCKING_PARSER=${PFX}/bin/ae-blocking-parser
    make install
fi
