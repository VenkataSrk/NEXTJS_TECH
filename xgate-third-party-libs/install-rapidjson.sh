#rapidjson is a header only file, so just checking the installed .h file
if [ -f /usr/local/include/rapidjson/document.h ]; then
	echo "rapidjson is alredy installed"
	exit 0
fi

echo "Building & installing rapidjson header only library ..."
$CPUs=$1

cd rapidjson

if [ ! -d "build" ]; then
	mkdir build
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
	echo "Failed to install redis-plus-plus"
	exit 1
fi

echo "installed" > $LIB_INSTALL_STATUS_FILE

