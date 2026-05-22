# ============================
# STAGE 1: BUILDER
# ============================
FROM ubuntu:24.04 AS builder
ENV URENGINE_BASE=/root
ENV SOFIA_DEBUG=9
ENV NUA_DEBUG=9
ENV SOA_DEBUG=9
ENV SDP_DEBUG=9
ENV NEA_DEBUG=9
ENV IPTSEC_DEBUG=9
ENV NTA_DEBUG=9
ENV TPORT_DEBUG=9
ENV TPORT_LOG=9
ENV TPORT_DUMP=1
ENV SU_DEBUG=9
# ----------------------------
# Base build tools
# ----------------------------
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    gcc g++ make \
    wget curl \
    pkg-config \
    meson ninja-build \
    libssl-dev \
    libreadline-dev \
    zlib1g-dev \
    bzip2 \
    libmount-dev \
    xsltproc \
    docbook-xsl \
    m4 \
    unixodbc unixodbc-dev \
    tar xz-utils \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /sgw

# ----------------------------
# Copy full source
# ----------------------------
COPY . /sgw
WORKDIR /sgw

# ----------------------------
# Extract all archives
# ----------------------------
WORKDIR /sgw/xg_third_party_libs
RUN find . -name '*.tar.xz' -exec tar -xvf {} \; && \
    find . -name '*.tar.gz' -exec tar -xvzf {} \; && \
    find . -name '*.tar.bz*' -exec tar -xvf {} \; && \
    find . -name '*.tgz' -exec tar -xvf {} \;

# ----------------------------
# 1. libffi
# ----------------------------
WORKDIR libffi-3.4.2
RUN ./configure --prefix=/usr/local --disable-static && \
    make -j$(nproc) && make install

# ----------------------------
# 2. pcre2
# ----------------------------
WORKDIR /sgw/xg_third_party_libs/pcre2-10.40
RUN ./configure --prefix=/usr/local && \
    make -j$(nproc) && make install

# ----------------------------
# 3. glib
# ----------------------------
WORKDIR /sgw/xg_third_party_libs/glib-2.72.0
RUN meson setup builddir -Dgtk_doc=false && \
    ninja -C builddir -j$(nproc) && \
    ninja -C builddir install

# ----------------------------
# 4. autoconf
# ----------------------------
WORKDIR /sgw/xg_third_party_libs/autoconf-2.69
RUN ./configure --prefix=/usr/local && \
    make -j$(nproc) && make install

# ----------------------------
# 5. automake
# ----------------------------
WORKDIR /sgw/xg_third_party_libs/automake-1.15
RUN ./configure --prefix=/usr/local && \
    make -j$(nproc) && make install

# ----------------------------
# 6. libtool
# ----------------------------
WORKDIR /sgw/xg_third_party_libs/libtool-2.4
RUN ./configure --prefix=/usr/local && \
    make -j$(nproc) && make install

# ----------------------------
# 7. ACE Wrappers
# ----------------------------
WORKDIR /sgw/xg_third_party_libs/

ENV ACE_ROOT=/sgw/xg_third_party_libs/ACE_wrappers

RUN cd ACE_wrappers && \
    echo '#include "ace/config-linux.h"' > ace/config.h && \
    echo 'include $(ACE_ROOT)/include/makeinclude/platform_linux.GNU' > include/makeinclude/platform_macros.GNU && \
    echo 'INSTALL_PREFIX = /usr/local' >> include/makeinclude/platform_macros.GNU && \
    make -j$(nproc) && \
    make install

# ----------------------------
# 8. unixODBC
# ----------------------------
WORKDIR /sgw/xg_third_party_libs/unixODBC-2.3.11
RUN ./configure --prefix=/usr/local && \
    make -j$(nproc) && make install

# ----------------------------
# 9. MySQL ODBC
# ----------------------------
WORKDIR /sgw/xg_third_party_libs/

RUN dpkg -i mysql-community-client-plugins_8.0.34-1ubuntu22.04_amd64.deb

RUN dpkg -i mysql-connector-odbc_8.0.32-1ubuntu22.04_amd64.deb

# ----------------------------
# 10. sofia-sip
# ----------------------------
WORKDIR /sgw/xg_third_party_libs/sofia-sip-1.13.17
RUN ./bootstrap.sh && \
    ./configure && \
    make -j$(nproc) && make install

RUN ldconfig
# ----------------------------
# Build SGW
# ----------------------------
WORKDIR /sgw
RUN mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Debug .. && \
    make -j$(nproc)

RUN mkdir -p /var/log/xgate/log
RUN mkdir -p /var/log/xgate/wss
ENTRYPOINT ["sh", "-c", "/sgw/build/urengine_wss >> /var/log/xgate/wss/urengine_wss.out 2>> /var/log/xgate/wss/urengine_wss.err"]
