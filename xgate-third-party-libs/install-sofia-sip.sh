OS_NAME=$1

if [[ -f /usr/local/lib/libsofia-sip-ua.a || -f /usr/local/lib64/libsofia-sip-ua.a ]]; then
	echo "Sofia SIP Stack is already installed"
  exit 0
fi

echo "Installing Sofia SIP Stack Library ..."

cd sofia-sip/

#cp su.c 1.13.9/libsofia-sip-ua/su/ #enabled it when required
#cp nta.c 1.13.9/libsofia-sip-ua/nta/ #will overwrite new changes
cp soa_tag.h 1.13.9/libsofia-sip-ua/soa/sofia-sip/

cd 1.13.9/
make clean

sh autogen.sh
./configure --with-openssl --disable-shared --without-doxygen --with-pic --enable-sctp
#./configure --with-openssl --disable-shared --without-doxygen --with-pic --disable-tag-cast
make

echo "Installing for user-->$USER ..."
if [ "$USER" == "root" ]; then
	make install
else
	echo "Enter root password to install:"
	sudo make install

	install_status=$?

	#Making sure we will not link to old or previously installed sofia lib.
	rm -f /usr/local/lib/libsofia-sip-ua.so*
	rm -f /usr/local/lib64/libsofia-sip-ua.so*
fi

exit $install_status

