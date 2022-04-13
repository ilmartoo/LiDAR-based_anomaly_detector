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

if [ $# = 1 ] && [ $1 = "-t" ]; then
	if [ ! -r lib/googletest/lib/libtest_main.a ] || [ ! -r lib/googletest/lib/libtest.a ] || [ ! -r lib/googletest/lib/libmock_main.a ] || [ ! -r lib/googletest/lib/libmock.a ]; then
		echo -e "\e[1;33m[gtest & gmock] Creando...\e[0m"
		cmake -S lib/googletest -B lib/googletest/build
		cmake --build lib/googletest/build
		mkdir -p lib/googletest/lib/include/
		mv lib/googletest/build/lib/* lib/googletest/lib/
		cp -r lib/googletest/googletest/include/* lib/googletest/lib/include/
		cp -r lib/googletest/googlemock/include/* lib/googletest/lib/include/
		echo -e "\e[1;32m[gtest & gmock] Librerías creadas\e[0m"
	else
		echo -e "\e[1;32m[gtest & gmock] Librerías ya disponibles\e[0m"
	fi
fi
