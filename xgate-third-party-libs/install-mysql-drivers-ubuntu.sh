<<COMMENT
Installation Link --> 	https://dev.mysql.com/doc/connector-odbc/en/connector-odbc-installation.html
			https://dev.mysql.com/doc/connector-odbc/en/connector-odbc-installation-source-unix.html

MySQL Community Downloads 	--> https://dev.mysql.com/downloads/

MySQL ODBC Connector Downloads 	--> https://www.mysql.com/products/connector/

Our Installation Choice/Path
----------------------------

* Connector/ODBC Installation
  -->https://dev.mysql.com/doc/connector-odbc/en/connector-odbc-installation.html

	||
	\/

* Installing Connector/ODBC on Unix-like Systems
  -->https://dev.mysql.com/doc/connector-odbc/en/connector-odbc-installation-binary-unix.html
COMMENT

echo "Installing prerequsite packages for installing MySQL Driver in Ubuntu ..."
read

if [[ -f /usr/local/include/sqlext.h || -f /usr/include/sqlext.h ]]; then
	echo "ODBC prequisite packages are already installed"
else
	echo "Installing 'unixodbc' & 'unixodbc-dev' in Ubuntu ..."
	apt-get install unixodbc unixodbc-dev
fi

MYSQL_COMMUNITY_CLIENT_PLUGIN_FILE="mysql-community-client-plugins_8.0.32-1ubuntu22.04_amd64.deb"
MYSQL_CONNECTOR_ODBC="mysql-connector-odbc_8.0.32-1ubuntu22.04_amd64.deb"
ODBC_INST_INI_FILE=/usr/local/etc/odbcinst.ini

if [ -f $ODBC_INST_INI_FILE ]; then
	echo "My SQL is already installed"
	exit 0
fi

cd /usr/local/xgate/

if [ -f $MYSQL_COMMUNITY_CLIENT_PLUGIN_FILE ]; then
	echo "$MYSQL_COMMUNITY_CLIENT_PLUGIN_FILE is already available/downloaded"
else
	echo "Downloading $MYSQL_COMMUNITY_CLIENT_PLUGIN_FILE pkg ..."
  wget --no-check-certificate https://dev.mysql.com/get/Downloads/MySQL-8.0/$MYSQL_COMMUNITY_CLIENT_PLUGIN_FILE
	echo "Installing $MYSQL_COMMUNITY_CLIENT_PLUGIN_FILE pkg ..."
  dpkg --install  $MYSQL_COMMUNITY_CLIENT_PLUGIN_FILE
fi

if [ -f $MYSQL_CONNECTOR_ODBC ]; then
	echo "$MYSQL_CONNECTOR_ODBC is already available/downloaded"
else
	echo "Downloading mysql-connector-odbc pkg ..."
	wget --no-check-certificate  https://dev.mysql.com/get/Downloads/Connector-ODBC/8.0/$MYSQL_CONNECTOR_ODBC
fi

if [ $? != 0 ]; then
	echo "Failed to download $MYSQL_CONNECTOR_ODBC "
	exit 1
fi

echo "Installing mysql-connector-odbc pkg ..."
dpkg --install $MYSQL_CONNECTOR_ODBC

if [ $? != 0 ]; then
	echo "Failed to install $MYSQL_CONNECTOR_ODBC "
	exit 1
fi

touch $ODBC_INST_INI_FILE

if [ -f $ODBC_INST_INI_FILE ]; then
	echo "Updating $ODBC_INST_INI_FILE with driver details ..."
	read

	echo "[MySQL ODBC 8.0 Unicode Driver]" >> $ODBC_INST_INI_FILE
	echo "Driver=/usr/lib/x86_64-linux-gnu/odbc/libmyodbc8w.so" >> $ODBC_INST_INI_FILE
	echo "UsageCount=1" >> $ODBC_INST_INI_FILE

	echo "" >> $ODBC_INST_INI_FILE
	echo "[MySQL ODBC 8.0 ANSI Driver]" >> $ODBC_INST_INI_FILE
	echo "Driver=/usr/lib/x86_64-linux-gnu/odbc/libmyodbc8w.so" >> $ODBC_INST_INI_FILE
	echo "UsageCount=1" >> $ODBC_INST_INI_FILE
else
	echo "Failed to install My SQL Driver. Missing file --> $ODBC_INST_INI_FILE"
fi

cd -

echo "------End of My SQL Driver Installation-----------------------"
echo

