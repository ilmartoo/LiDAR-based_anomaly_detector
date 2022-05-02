#!/bin/bash

# Project root directory
root="$(pwd)/$(dirname $0)/.."

# Librerias sdk_core y read_lvx
${root}/anomaly_detection/setup_libraries.sh

# Instala la librería OpenBLAS
echo "int main(){}" | g++ -o /dev/null -x c - -lopenblas 2>/dev/null
if [ $? -eq 1 ]; then
    echo -e "\e[1;33m[openblas] OpenBLAS is needed for armadillo to work properly!\e[0m"

    sudo apt-get install libopenblas-dev

    echo -e "\e[1;32m[openblas] Library installed\e[0m"
else
    echo -e "\e[1;32m[openblas] Library already installed\e[0m"
fi

# Crea la librería armadillo
if [ ! -r "${root}/libs/lib/libarmadillo.so" ]; then
    echo -e "\e[1;33m[armadillo] Creating library...\e[0m"

    cmake -S "${root}/bin/armadillo/" -B "${root}/bin/armadillo/build" -D CMAKE_INSTALL_PREFIX:PATH="${root}/libs"
    make -C "${root}/bin/armadillo/build"
    make install -C "${root}/bin/armadillo/build"

    echo -e "\e[1;32m[armadillo] Library created\e[0m"
else
    echo -e "\e[1;32m[armadillo] Library already installed\e[0m"
fi
