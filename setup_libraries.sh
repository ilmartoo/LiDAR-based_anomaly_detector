# Crea la librería sdk_core
if [ ! -r /usr/local/lib/liblivox_sdk_static.a ]; then
	cd sdk_core/ && mkdir -p build
	cd build && cmake ..
	make && sudo make install
	cd ../../
	echo -e "\e[1;34m[sdk_core] Librería sdk_corecreada\e[0m"
fi

# Crea la librería googletest
if [ ! -r /usr/local/lib/libgmock.a ] && [ ! -r /usr/local/lib/libgtest.a ]; then
	cd googletest/ && mkdir -p build
	cd build && cmake ..
	make && sudo make install
	cd ../../
	echo -e "\e[1;34m[sdk_core] Librería sdk_core creada\e[0m"
fi