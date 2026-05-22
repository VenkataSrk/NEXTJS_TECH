#!/bin/bash

OS_NAME=$1
CPUs=`nproc`

if [[ -f /usr/local/lib/libhiredis.a || -f /usr/local/lib64/libhiredis.a ]]; then
	echo "libhiredis is already installed"
elif [[ -f /usr/local/lib/libhiredis.so || -f /usr/local/lib64/libhiredis.so ]]; then
	echo "libhiredis is already installed"
else

	if [ -d "hiredis" ]; then
		cd hiredis/
	else
		echo "hiredis is not yet cloned. Aborting futher processing ..."
		exit 1
	fi

	if [ ! -d "build" ]; then
		mkdir build
	else
		rm -rf build/*
	fi

	cd build
	cmake ..
	make -j$CPUs

	if [ "$USER" == "root" ]; then
		make install
	else
		echo "Enter root password to install:"
		sudo make install
	fi

	if [ $? != 0 ]; then
		echo "Failed to install hiredis"
		exit
	fi
  echo "installed" > $INSTALL_STATUS_FILE
  echo
	echo "successfully installed-->libhiredis"

	#Cannot delete .so file in Ubuntu as it looks for libhiredis.so.1.0.3-dev package

  if [ "$OS_NAME" == "CentOS" ]; then
    rm -f /usr/local/lib/libhiredis.so*
    rm -f /usr/local/lib64/libhiredis.so*
  fi
fi
echo "--------------------------------------------------------------------------"

