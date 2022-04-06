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
	# if [ ! -r lib/read_lvx/lib/libread_lvx.a ]; then
	# 	echo -e "\e[1;33m[gtest] Creando...\e[0m"
	# 	cmake -S lib/read_lvx -B lib/read_lvx/build
	# 	cmake --build lib/read_lvx/build -- install
	# 	echo -e "\e[1;32m[gtest] Librería creada\e[0m"
	# else
	# 	echo -e "\e[1;32m[gtest] Librería ya disponible\e[0m"
	# fi
	echo g
fi