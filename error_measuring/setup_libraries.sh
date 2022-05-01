#!/bin/bash

# Project root directory
root="$(dirname $0)/.."

# Librerias sdk_core y read_lvx
${root}/src/anomaly_detector/setup_libraries.sh

# Instala la librería OpenBLAS
echo "int main(){}" | g++ -o /dev/null -x c - -lopenblas 2>/dev/null
if [ $? -eq 1 ]; then
    sudo apt-get install libopenblas-dev
fi

# Crea la librería armadillo
if [ ! -r "${root}/lib/armadillo/lib/libarmadillo.so" ]; then
    echo -e "\e[1;33m[armadillo] Creando...\e[0m"
    cmake -S "${root}/lib/armadillo/" -B "${root}/lib/armadillo/build"
    cmake --build "${root}/lib/armadillo/build"
    mkdir -p "${root}/lib/armadillo/lib/"
    mv "${root}/lib/armadillo/build/libarmadillo.so*" "${root}/lib/armadillo/lib/"
    echo -e "\e[1;32m[armadillo] Librería creada\e[0m"
else
    echo -e "\e[1;32m[armadillo] Librería ya disponible\e[0m"
fi
