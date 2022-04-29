#!/bin/bash

root="../../"

# Crea la librería sdk_core
if [ ! -r "${root}/lib/sdk_core/lib/liblivox_sdk_static.a" ]; then
    echo -e "\e[1;33m[sdk_core] Creando...\e[0m"
    cmake -S "${root}lib/sdk_core" -B "${root}lib/sdk_core/build"
    cmake --build "${root}lib/sdk_core/build" -- install
    echo -e "\e[1;32m[sdk_core] Librería creada\e[0m"
else
    echo -e "\e[1;32m[sdk_core] Librería ya disponible\e[0m"
fi

# Crea la librería read_lvx
if [ ! -r "${root}lib/read_lvx/lib/libread_lvx.a" ]; then
    echo -e "\e[1;33m[read_lvx] Creando...\e[0m"
    cmake -S "${root}lib/read_lvx" -B "${root}lib/read_lvx/build"
    cmake --build "${root}lib/read_lvx/build" -- install
    echo -e "\e[1;32m[read_lvx] Librería creada\e[0m"
else
    echo -e "\e[1;32m[read_lvx] Librería ya disponible\e[0m"
fi
