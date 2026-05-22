#!/bin/bash
### Author : Sathguru ###

cleanup()
{
  echo "Received Ctrl+C signal from user. Exit!"
  exit 1
}
trap cleanup SIGINT

SOURCE_PATH=$(pwd)
cd /usr/local/src

libpath=("/usr/lib" "/usr/lib64" "/usr/local/lib" "/usr/local/lib64" "/opt/boost/lib")

#Install HiRedis

if find "${libpath[@]}" -name "libhiredis.so" -print -quit | grep -q "."; then
	echo "hiredis already installed"
else
    rm -rf hiredis
    git clone https://github.com/redis/hiredis.git
    cd hiredis
    make -j$(nproc)
    make install -j$(nproc)
    cd ../
fi

#Install Redis++

if find "${libpath[@]}" -name "libredis++.so" -print -quit | grep -q "."; then
	echo "Redis++ already installed"
else
    rm -rf redis-plus-plus
    git clone https://github.com/sewenew/redis-plus-plus.git
    cd redis-plus-plus
    mkdir build
    cd build
    cmake -DREDIS_PLUS_PLUS_CXX_STANDARD=11 ..
    make -j$(nproc)
    make install -j$(nproc)
    cd ..
fi

#Install Boost

if find "${libpath[@]}" -name "libboost_filesystem.so" -print -quit | grep -q "."; then
	echo "Boost already installed"
else
    rm -rf boost_1_84_0
    wget https://archives.boost.io/release/1.84.0/source/boost_1_84_0.tar.gz --no-check-certificate
    tar -xzvf boost_1_84_0.tar.gz
    cd boost_1_84_0/
    ./bootstrap.sh --with-toolset=gcc --prefix=/opt/boost
    ./b2 toolset=gcc -j$(nproc)
    ./b2 install -j$(nproc)
fi
