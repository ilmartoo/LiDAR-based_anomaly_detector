cd lib/ # Entramos a la carpeta de librerías

# Crea la librería sdk_core
if [ ! -r /usr/local/lib/liblivox_sdk_static.a ]; then
	cd sdk_core/ && mkdir -p build && cd build && cmake .. && make && sudo make install && cd ../../
	echo -e "\e[1;34m[sdk_core] Librería sdk_core creada\e[0m"
else
	echo -e "\e[1;34m[sdk_core] Librería sdk_core ya disponible\e[0m"
fi

# Crea la librería googletest
if [ ! -r /usr/local/lib/libgmock.a ] && [ ! -r /usr/local/lib/libgtest.a ]; then
	cd googletest/ && mkdir -p build && cd build && cmake .. && make && sudo make install && cd ../../
	echo -e "\e[1;34m[sdk_core] Librerías gtest y gmock creadas\e[0m"
else
	echo -e "\e[1;34m[sdk_core] Librerías gtest y gmock ya disponibles\e[0m"
fi

# Crea la librería read_lvx
if [ ! -r /usr/local/lib/read_lvx.a ]; then
	cd read_lvx/ && mkdir -p build && cd build && cmake .. && make && sudo make install && cd ../../
	echo -e "\e[1;34m[sdk_core] Librería read_lvx creada\e[0m"
else
	echo -e "\e[1;34m[sdk_core] Librería read_lvx ya disponible\e[0m"
fi

cd ../ # Salimos de la carpeta de librerías