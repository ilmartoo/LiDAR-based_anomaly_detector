#!/bin/bash

# Project root directory
root="$(cd "$(dirname "$0")" && pwd)/.."

# Crea la librería sdk_core
if [ ! -r "${root}/libs/lib/liblivox_sdk.a" ]; then
    echo -e "\e[1;33m[sdk_core] Creating library...\e[0m"

    cmake -S "${root}/bin/sdk_core" -B "${root}/bin/sdk_core/build" -D CMAKE_INSTALL_PREFIX="${root}/libs"
    make -C "${root}/bin/sdk_core/build"
    make install -C "${root}/bin/sdk_core/build"
    
    echo -e "\e[1;32m[sdk_core] Library created\e[0m"
else
    echo -e "\e[1;32m[sdk_core] Library already installed\e[0m"
fi

# Crea la librería read_lvx
if [ ! -r "${root}/libs/lib/libread_lvx.a" ]; then
    echo -e "\e[1;33m[read_lvx] Creating library...\e[0m"

    cmake -S "${root}/bin/read_lvx" -B "${root}/bin/read_lvx/build" -D CMAKE_INSTALL_PREFIX="${root}/libs"
    make -C "${root}/bin/read_lvx/build"
    make install -C "${root}/bin/read_lvx/build"
    
    echo -e "\e[1;32m[read_lvx] Library created\e[0m"
else
    echo -e "\e[1;32m[read_lvx] Library already installed\e[0m"
fi
