# Instala las librerías OpenBLAS y LAPACK
echo "int main(){}" | g++ -o /dev/null -x c - -lopenblas -llapack 2>/dev/null
if [ $? -eq 1 ]; then
	echo -e "\e[1;32m[OpenBLAS & LAPACK] Creando...\e[0m"
	sudo apt-get install libopenblas-dev liblapack-dev
	echo -e "\e[1;32m[OpenBLAS & LAPACK] Librerías OpenBLAS y LAPACK instaladas\e[0m"
else
	echo -e "\e[1;32m[OpenBLAS & LAPACK] Librerías ya disponibles\e[0m"
fi

# Crea la librería armadillo
if [ ! -r lib/armadillo-11.0.0/lib/libarmadillo.so ]; then
	echo -e "\e[1;33m[armadillo] Creando...\e[0m"
	cmake -S lib/armadillo-11.0.0/ -B lib/armadillo-11.0.0/build
	cmake --build lib/armadillo-11.0.0/build
	mkdir -p lib/armadillo-11.0.0/lib/
	mv lib/armadillo-11.0.0/build/libarmadillo.so* lib/armadillo-11.0.0/lib/
	echo -e "\e[1;32m[armadillo] Librería creada\e[0m"
else
	echo -e "\e[1;32m[armadillo] Librería ya disponible\e[0m"
fi
