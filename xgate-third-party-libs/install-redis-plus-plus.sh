CPUs=`nproc`

if [[ -f /usr/local/lib/libredis++.a || -f /usr/local/lib64/libredis++.a ]]; then
	echo "libredis++ is already installed"
else

	cd redis-plus-plus/

	if [ ! -d "build" ]; then
		mkdir build
	else
		rm -rf build/*
	fi

	cd build
	cmake .. -DREDIS_PLUS_PLUS_CXX_STANDARD=20 -DREDIS_PLUS_PLUS_BUILD_TEST=OFF -DREDIS_PLUS_PLUS_BUILD_SHARED=OFF
	make -j$CPUs

	if [ "$USER" == "root" ]; then
		make install
	else
		echo "Enter root password to install:"
		sudo make install
	fi

	if [ $? != 0 ]; then
		echo "Failed to install redis-plus-plus"
		exit 1
	fi

  echo "installed" > $INSTALL_STATUS_FILE
  echo
	echo "successfully installed-->libredis++"

	rm -f /usr/local/lib/libredis++.so*
	rm -f /usr/local/lib64/libredis++.so*

fi

