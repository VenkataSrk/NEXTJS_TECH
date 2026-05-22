#!/bin/bash
set -e

#SRC_DIR="/usr/local/src"
THIRD_PARTY_DIR="$(pwd)/xg_third_party_libs"

echo "=== Installing build dependencies ==="
apt update
apt install -y \
    build-essential \
    cmake \
    pkg-config \
    meson \
    ninja-build \
    libreadline-dev \
    zlib1g-dev \
    bzip2 \
    libmount-dev \
    xsltproc \
    docbook-xsl \
    libssl-dev \
    m4 \
    unixodbc \
    unixodbc-dev \
    wget \

echo "=== Preparing source directory ==="
cd ${THIRD_PARTY_DIR}

echo "=== Extracting archives ==="
find . -name '*.tar.xz' -exec tar -xvf {} \;
find . -name '*.tar.bz*' -exec tar -xvf {} \;
find . -name '*.tar.gz' -exec tar -xvzf {} \;
find . -name '*.tgz' -exec tar -xvf {} \;

##################################
# 1. libffi
##################################
cd libffi-3.4.2
./configure --prefix=/usr/local --disable-static
make -j$(nproc)
make install
cd ..

##################################
# 2. pcre2
##################################
cd pcre2-10.40
./configure --prefix=/usr/local
make -j$(nproc)
make install
cd ..

##################################
# 3. glib
##################################
cd glib-2.72.0
meson setup builddir -Dgtk_doc=false
cd builddir
ninja -j$(nproc)
ninja install
cd ../..

##################################
# 5. autoconf
##################################
cd autoconf-2.69
./configure --prefix=/usr/local
make -j$(nproc)
make install
cd ..

##################################
# 6. automake
##################################
cd automake-1.15
./configure --prefix=/usr/local
make -j$(nproc)
make install
cd ..

##################################
# 7. libtool
##################################
cd libtool-2.4
./configure --prefix=/usr/local
make -j$(nproc)
make install
cd ..

##################################
# 8. ACE_wrappers
##################################
cd ACE_wrappers
export ACE_ROOT=${THIRD_PARTY_DIR}/ACE_wrappers
export LD_LIBRARY_PATH=$ACE_ROOT/lib:$LD_LIBRARY_PATH

cat > ${ACE_ROOT}/ace/config.h <<EOF
#include "ace/config-linux.h"
EOF

cat > ${ACE_ROOT}/include/makeinclude/platform_macros.GNU <<EOF
include \$(ACE_ROOT)/include/makeinclude/platform_linux.GNU
INSTALL_PREFIX = /usr/local
EOF

make -j$(nproc)
make install
cd ..

##################################
# 9. MySQL ODBC
##################################
cd unixODBC-2.3.11
./configure --prefix=/usr/local
make -j$(nproc)
make install
cd ..

#wget --no-check-certificate \
#https://repo.mysql.com/apt/ubuntu/pool/mysql-8.0/m/mysql-community/mysql-community-client-plugins_8.0.34-1ubuntu22.04_amd64.deb
dpkg -i mysql-community-client-plugins_8.0.34-1ubuntu22.04_amd64.deb

#wget --no-check-certificate \
#https://dev.mysql.com/get/Downloads/Connector-ODBC/8.0/mysql-connector-odbc_8.0.32-1ubuntu22.04_amd64.deb
dpkg -i mysql-connector-odbc_8.0.32-1ubuntu22.04_amd64.deb
##################################
# 10. sofia-sip
##################################
cd sofia-sip-1.13.17
./bootstrap.sh
./configure --prefix=/usr/local
make -j$(nproc)
make install
cd ..

##################################
# Final
##################################
ldconfig
echo "=== ALL THIRD PARTY LIBS INSTALLED SUCCESSFULLY ==="

