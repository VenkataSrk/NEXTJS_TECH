echo "ACE Installation Ref Link-->https://www.dre.vanderbilt.edu/~schmidt/DOC_ROOT/ACE/ACE-INSTALL.html"

ACE_INSTALL_DIR=/usr/local/xgate

ACE_MAJOR_VER=7
ACE_MINOR_VER=0
ACE_PATCH_VER=10
ACE_VERSION="$ACE_MAJOR_VER.$ACE_MINOR_VER.$ACE_PATCH_VER"

isAceInstalled()
{
	if [ -d $ACE_INSTALL_DIR ]; then
		echo "$ACE_INSTALL_DIR directory exists"
		if [ -f /usr/local/xgate/ACE_wrappers/ace/libACE.so ]; then
			echo "/usr/local/xgate/ACE_wrappers/ace/libACE.so file exists"
			if [ -f /usr/local/xgate/ACE_wrappers/ace/libACE.so.${ACE_VERSION} ]; then
				echo "/usr/local/xgate/ACE_wrappers/ace/libACE.so.${ACE_VERSION} file exists"
				return 1
			else
				echo "But required libACE.so.${ACE_VERSION} is not installed"
				return 0
			fi
		fi
	fi

	echo "----------------------------------------------------------------"
	return 0
}

isAceInstalled

if [ $? == 1 ]; then
	echo "ACE is already installed"
	exit 0
fi


echo "********** Going to install ACE *************"
CPUs=`nproc`
if [ "$CPUs" == "" ]; then
	CPUs=4
fi
#echo "Going to use $CPUs threads in make command"

if [ ! -d $ACE_INSTALL_DIR ]; then
	mkdir $ACE_INSTALL_DIR
fi

cd $ACE_INSTALL_DIR

echo "Searching required pkg in /root/ ..."
if [ -f /root/ACE-${ACE_VERSION}.tar.gz ]; then
	echo "Found required ACE pkg /root/ACE-${ACE_VERSION}.tar.gz"
	cp /root/ACE-${ACE_VERSION}.tar.gz .
elif [ -f /root/ACE-${ACE_VERSION}.zip ]; then
	cp /root/ACE-${ACE_VERSION}.zip .
else
	echo "Could not find ACE-${ACE_VERSION}.tar.gz or ACE-${ACE_VERSION}.zip in /root/"
	echo "You have the option to copy ACE-${ACE_VERSION}.tar.gz to /root/. Cntrl+C now & copy & then try again."
	echo "OR just enter to download it & install by this script"
  read
fi

if [ -f "ACE-${ACE_VERSION}.tar.gz" ]; then
	tar xvzf ACE-${ACE_VERSION}.tar.gz
elif [ -f "ACE-${ACE_VERSION}.zip" ]; then
	unzip ACE-${ACE_VERSION}.zip
else
	echo "No ACE package found"

	CMD="wget --no-check-certificate https://github.com/DOCGroup/ACE_TAO/releases/download/ACE%2BTAO-${ACE_MAJOR_VER}_${ACE_MINOR_VER}_${ACE_PATCH_VER}/ACE-${ACE_VERSION}.tar.gz"
	echo "Downloading-->$CMD"
	echo "Enter to continue"
	read
	$CMD

	if [ $? != 0 ]; then
		echo "ERROR:: Failed to download from https://github.com/DOCGroup/ACE_TAO/releases/download/ACE%2BTAO-7_0_10/ACE-${ACE_VERSION}.tar.gz"
		exit 1
	fi

	echo "Successfully downloaded ACE-${ACE_VERSION}.tar.gz ... Enter to continue"
	read

	if [ -f "ACE-${ACE_VERSION}.tar.gz" ]; then
		tar xvzf ACE-${ACE_VERSION}.tar.gz
	elif [ -f "ACE-${ACE_VERSION}.zip" ]; then
		unzip ACE-${ACE_VERSION}.zip
	else
		echo "No ACE package found"
		exit 1
	fi
fi

echo "GNU make 3.79.1 or higher is required. Enter to continue"
read

cd ACE_wrappers

export ACE_ROOT=$ACE_INSTALL_DIR/ACE_wrappers

echo "Creating a configuration file, $ACE_ROOT/ace/config.h ..."

cp $ACE_ROOT/debian/config.h $ACE_ROOT/ace/config.h

if [ $? != 0 ]; then
	echo "ERROR:: Failed to create $ACE_ROOT/ace/config.h file"
	exit 1
fi

echo "debug			= 0" >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo "optimize	= 1" >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo "inline		= 1" >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo "ssl				= 1" >> $ACE_ROOT/include/makeinclude/platform_macros.GNU

echo "include ${ACE_ROOT}/include/makeinclude/platform_linux.GNU" >> $ACE_ROOT/include/makeinclude/platform_macros.GNU

echo "INSTALL_PREFIX=/usr/local" >> $ACE_ROOT/include/makeinclude/platform_macros.GNU

if [ $? != 0 ]; then
	echo "ERROR:: Failed to create $ACE_ROOT/include/makeinclude/platform_macros.GNU file"
	exit 1
fi

export LD_LIBRARY_PATH=$ACE_ROOT/lib:$LD_LIBRARY_PATH

make -j$CPUs

if [ $? != 0 ]; then
	echo "ERROR:: make failed"
	exit 1
fi

make install

echo "installed" > $ACE_INSTALL_DIR/.install_status_ACE


