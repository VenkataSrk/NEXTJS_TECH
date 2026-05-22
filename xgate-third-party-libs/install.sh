#---------------Common variables ---------------------------------------------
CPUs=`nproc`
PROJECT_ROOT_DIR=`pwd`
THIRD_PARTY_DIR=xgate-third-party-libs
PKG_DOWNLOAD_DIR=/usr/local/xgate/

if [ "$CPUs" == "" ]; then
	CPUs=4
fi

#-----------------------------------------------------------------------------

function install_cmake()
{
	echo "Installing cmake 3.26 ..."

	cd $PKG_DOWNLOAD_DIR

	wget https://github.com/Kitware/CMake/releases/download/v3.26.0/cmake-3.26.0.tar.gz

	tar -xvzf cmake-3.26.0.tar.gz

	cd cmake-3.26.0

	./bootstrap

	make

	make install

	cd -

	exit 0
}

#----------Identifying OS-----------------------------------------------------
grep NAME /etc/os-release | grep Ubuntu
Is_OS_Ununtu=$?
echo "Is_OS_Ununtu-->$Is_OS_Ununtu"
grep NAME /etc/os-release | grep CentOS
Is_OS_CentOS=$?
echo "Is_OS_CentOS-->$Is_OS_CentOS"

OS_NAME=""

if [ $Is_OS_Ununtu == 0 ]; then
	OS_NAME="Ubuntu"


	if [ ! -f /usr/bin/g++ ]; then
	  echo "Installing build-essential in Ubuntu ..."
	  apt install build-essential
	fi

	echo "Installing 'make' & 'cmake' in Ubuntu ..."
	if [ ! -f /usr/bin/make ]; then
	  apt-get install make
	fi

	if [ ! -f /usr/bin/cmake ]; then
	  apt-get install cmake
	fi

	apt install pkg-config
	apt install libtool libtool-bin
	apt-get install automake
	apt-get install autoconf

	apt install libcurl4-openssl-dev

	#apt-get upgrade gcc
elif [ $Is_OS_CentOS == 0 ]; then
	OS_NAME="CentOS"
	echo "Installing 'make' & 'cmake' in CentOS ..."

	if [ ! -f /usr/bin/make ]; then
		echo "Installing make in CentOS"
		yum install make
	fi

	if [[ -f /usr/bin/cmake || -f  /usr/local/bin/cmake ]]; then
			echo "Checking which cmake version is already installed"
			if [ `cmake --version | grep "cmake version" | grep "3.22|3.23|3.24|3.25|3.26"` ]; then
					echo "Desired cmake version is not installed."
					install_cmake
			else
					echo "Desired cmake version is already installed"
					cmake --version
			fi
	else
			echo "Installating desired cmake version in CentOS"
			install_cmake
	fi

else
	echo "Cannot install uuid-dev package. Unsupported OS"
	exit 1
fi

#-----------------------------------------------------------------------------
<<COMMENTED
function install_automake()
{
    [ $# -eq 0 ] && { run_error "Usage: install_automake <version>"; exit; }
    local VERSION=${1}
    wget https://ftp.gnu.org/gnu/automake/automake-${VERSION}.tar.gz
    if [ -f "automake-${VERSION}.tar.gz" ]; then
            tar -xzf automake-${VERSION}.tar.gz
            cd automake-${VERSION}/
            ./configure
            make && make install
            echo -e "\e[1;39m[   \e[1;32mOK\e[39m   ] automake-${VERSION} installed\e[0;39m"

	    rm automake-1.16.tar.gz
	    rm -f automake-1.16/
    else
	    echo -e "\e[1;39m[   \e[31mError\e[39m   ] cannot fetch file from ftp://ftp.gnu.org/gnu/automake/ \e[0;39m"
	    exit 1
    fi
}
install_automake 1.16

if [ $? != 0 ]; then
	echo "********Error while installing automake packages !***********"
	echo
	exit 1
fi
COMMENTED
#-----------------------------------------------------------------------------


echo "-------------------------------------------------------------------------"
echo "Setting up build system for PBX in directory-->$PROJECT_ROOT_DIR"
echo "Going to use $CPUs threads in make command"
echo "-------------------------------------------------------------------------"

echo
echo "---------------Updating dependent git submodules-------------------------"
git submodule init
git submodule update
echo "-------------------------------------------------------------------------"

#git clean -fdx
rm `find ./ -name *.o`
rm `find ./ -name *.a`

echo
echo "----------------OpenSSL Installation-------------------------------------"
cd $PROJECT_ROOT_DIR

cd $THIRD_PARTY_DIR
./install-openssl-3.sh $OS_NAME
echo "-------------------------------------------------------------------------"

echo
echo "---------------ACE Installation------------------------------------------"
cd $PROJECT_ROOT_DIR

cd $THIRD_PARTY_DIR
./install-ACE.sh $CPUs
echo "-------------------------------------------------------------------------"

echo
echo "----------------Sofia SIP Stack Installation-----------------------------"
cd $PROJECT_ROOT_DIR
cd $THIRD_PARTY_DIR
./install-sofia-sip.sh $CPUs
echo "-------------------------------------------------------------------------"

echo
echo "----------------rapidxml Installation-------------------------------------"
cd $PROJECT_ROOT_DIR
cd $THIRD_PARTY_DIR
#cp rapidxml_print.hpp rapidxml/

echo
echo "----------------hiredis Installation-------------------------------------"
cd $PROJECT_ROOT_DIR
cd $THIRD_PARTY_DIR

./install-hiredis.sh $OS_NAME

echo "--------------------------------------------------------------------------"

echo
echo "-----------------libredis++ Installation----------------------------------"
cd $PROJECT_ROOT_DIR
cd $THIRD_PARTY_DIR

./install-redis-plus-plus.sh $OS_NAME

echo "-----------------------------------------------------------------------------"

echo
echo "-----------------rapidjson Installation--------------------------------------"
cd $PROJECT_ROOT_DIR
cd $THIRD_PARTY_DIR

./install-rapidjson.sh $CPUs

echo "-----------------------------------------------------------------------------"

echo "-----------------Building xGateLogger library--------------------------------"
cd $PROJECT_ROOT_DIR
cd xgate-frameworks/xgate-logger/

if [ -f ./lib/libxGateLogger.a ]; then
	echo "libxGateLogger lib is already built"
else

	if [ ! -d "build" ]; then
		mkdir build
	else
		rm -rf build/* lib/
	fi

	cd build
	cmake .. -DCMAKE_BUILD_TYPE=Debug #Release|Debug
	make clean
	make
	make install
fi
echo "-----------------------------------------------------------------------------"

echo "-----------------Building xGateUtilities library--------------------------------"
cd $PROJECT_ROOT_DIR
cd xgate-utilities

if [ -f lib/libxGateUtilities.a ]; then
	echo "libxGateLogger lib is already built"
else

	if [ ! -d "build" ]; then
		mkdir build
	else
		rm -rf build/* lib/
	fi

	cd build
	cmake .. -DCMAKE_BUILD_TYPE=Debug #Release|Debug
	make clean
	make
	make install
fi
echo "-----------------------------------------------------------------------------"

echo "-----------------Building NetworkMgr library--------------------------------"
cd $PROJECT_ROOT_DIR
cd xgate-frameworks/network-mgr/

if [ -f lib/libNetworkMgr.a ]; then
	echo "libNetworkMgr lib is already built"
else

	if [ ! -d "build" ]; then
		mkdir build
	else
		rm -rf build/* lib/
	fi

	cd build
	cmake .. -DCMAKE_BUILD_TYPE=Debug #Release|Debug
	make clean
	make
	make install
fi
echo "-----------------------------------------------------------------------------"

echo "-----------------Building xGateRedisClient library---------------------------"
cd $PROJECT_ROOT_DIR
cd xgate-frameworks/redis-client-mgr/

if [ -f lib/libxGateRedisClient.a ]; then
	echo "xGateRedisClient lib is already built"
else

	if [ ! -d "build" ]; then
		mkdir build
	else
		rm -rf build/* lib/
	fi

	cd build
	cmake .. -DCMAKE_BUILD_TYPE=Debug #Release|Debug
	make clean
	make
	make install
fi
echo "-----------------------------------------------------------------------------"

echo "-----------------Building xGateThreadPool library---------------------------"
cd $PROJECT_ROOT_DIR
cd xgate-frameworks/thread-pool/

if [ -f lib/libxGateThreadPool.a ]; then
	echo "xGateThreadPool lib is already built"
else

	if [ ! -d "build" ]; then
		mkdir build
	else
		rm -rf build/*
	fi

	cd build
	cmake .. -DCMAKE_BUILD_TYPE=Debug #Release|Debug
	make clean
	make
	make install
fi
echo "-----------------------------------------------------------------------------"

if [ -f /usr/include/uuid/uuid.h ]; then
	echo "uuid-dev package is already installed"
else
	if [ $Is_OS_Ununtu == 0 ]; then
		echo "Installing uuid-dev in Ubuntu ..."
		apt-get install uuid-dev
	elif [ $Is_OS_CentOS == 0 ]; then
		echo "Installing uuid-dev in Ubuntu ..."
		yum install 
	else
		echo "Cannot install uuid-dev package. Unsupported OS"
		exit 1
	fi
fi

if [[ -f /usr/local/include/sqlext.h || -f /usr/include/sqlext.h ]]; then
	echo "ODBC dev package is already installed"
else
	if [ $Is_OS_Ununtu == 0 ]; then
		echo "Installing 'unixodbc' & 'unixodbc-dev' in Ubuntu ..."
		apt-get install unixodbc unixodbc-dev
	elif [ $Is_OS_CentOS == 0 ]; then
		echo "Installing 'unixodbc' & 'unixodbc-dev' in CentOS ..."
		yum install unixODBC-devel
	else
		echo "Cannot install uuid-dev package. Unsupported OS"
		exit 1
	fi
fi

echo "Building PBX code base with $CPUs threads ... Enter to continue"
read
cd $PROJECT_ROOT_DIR
if [ -d "$PROJECT_ROOT_DIR/build" ]; then
rm -rf build
fi
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ../
make clean; 
make -j$CPUs

rm -f `find ./ -name *.o`

