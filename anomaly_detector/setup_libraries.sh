# Crea la librería sdk_core
if [ ! -r lib/sdk_core/lib/liblivox_sdk_static.a ]; then
	echo -e "\e[1;33m[sdk_core] Creando...\e[0m"
	cmake -S lib/sdk_core -B lib/sdk_core/build
	cmake --build lib/sdk_core/build -- install
	echo -e "\e[1;32m[sdk_core] Librería creada\e[0m"
else
	echo -e "\e[1;32m[sdk_core] Librería ya disponible\e[0m"
fi

# Crea la librería read_lvx
if [ ! -r lib/read_lvx/lib/libread_lvx.a ]; then
	echo -e "\e[1;33m[read_lvx] Creando...\e[0m"
	cmake -S lib/read_lvx -B lib/read_lvx/build
	cmake --build lib/read_lvx/build -- install
	echo -e "\e[1;32m[read_lvx] Librería creada\e[0m"
else
	echo -e "\e[1;32m[read_lvx] Librería ya disponible\e[0m"
fi

# Creas las librerías googletest y googlemock
if [ $# = 1 ] && [ $1 = "-t" ]; then
	if [ ! -r lib/googletest-1.11.0/lib/libtest_main.a ] || [ ! -r lib/googletest-1.11.0/lib/libtest.a ] || [ ! -r lib/googletest-1.11.0/lib/libmock_main.a ] || [ ! -r lib/googletest-1.11.0/lib/libmock.a ]; then
		echo -e "\e[1;33m[gtest & gmock] Creando...\e[0m"
		cmake -S lib/googletest-1.11.0 -B lib/googletest-1.11.0/build
		cmake --build lib/googletest-1.11.0/build
		mkdir -p lib/googletest-1.11.0/lib/include/
		mv lib/googletest-1.11.0/build/lib/* lib/googletest-1.11.0/lib/
		cp -r lib/googletest-1.11.0/googletest/include/* lib/googletest-1.11.0/lib/include/
		cp -r lib/googletest-1.11.0/googlemock/include/* lib/googletest-1.11.0/lib/include/
		echo -e "\e[1;32m[gtest & gmock] Librerías creadas\e[0m"
	else
		echo -e "\e[1;32m[gtest & gmock] Librerías ya disponibles\e[0m"
	fi
fi
