OS_NAME=$1
CPUs=`nproc`

if [[ -f /usr/local/lib/libssl.so.3 || -f /usr/local/lib64/libssl.so.3 ]]; then
	echo "OpenSSL i.e., libssl.so.3 is already installed"
	exit 0
fi

echo "Installing OpenSSL 3 or higher"

if [ "$OS_NAME" == "Ubuntu" ]; then
  apt install libssl-dev
else
  cd $THIRD_PARTY_DIR

  git clone  https://github.com/openssl/openssl.git

  cd openssl/

  ./Configure
  make -j$CPUs
  make install

fi
