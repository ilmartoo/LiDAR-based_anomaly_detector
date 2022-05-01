# Error Measuring

Project to measure the cuadratic error a predefined LiDAR data may have using plane computation.

*...in-depth explanation to come...*

## Prerequisites

To compile the project you will need:

- `g++` version `9.4.0` or higher
- `cmake` version `14.0` or higher

We also need this libraries for the project to work:

- [livox-sdk 2.3.0](https://github.com/Livox-SDK/Livox-SDK/releases/tag/v2.3.0)
- [read_lvx](https://github.com/michalpelka/read_lvx)
- [armadillo 10.0.0](https://sourceforge.net/projects/arma/files/)

_Note: `read_lvx` repository do not have a CMakeList.txt to generate a library, but an executable, so it needs to be created._

In the `libraries.zip` are located **minimalist versions of the needed libraries with some modifications** so they can installed without having to modify the libraries any further.

## Developing enviroment

The project was developed and tested in `Ubuntu 20.04.4` and `Ubuntu 21.04`.

## Setup

### Libraries

To setup the necesary libraries you can use the script `setup_libraries.sh` from the **project root folder**:

```bash
# Uncompress modified libraries from zip
unzip libraries.zip –d /lib
# Setup needed libraries
./error_measuring/setup_libraries.sh
```

This script will create and install the needed libraries into their corresponding `lib/` subfolder, keeping the libraries from being installed into the `/usr/local/`.
### Project

To build de project execute the following from the **project root folder**:

```bash
# Build the project
cmake -S error_measuring -B build/error_measuring
cmake --build build/error_measuring
```

To build the project with debug output build it with `CMAKE_BUILD_TYPE` option set to `Debug`:

```bash
# Build the project with debug output
cmake -S error_measuring -B build/error_measuring -D CMAKE_BUILD_TYPE=Debug
cmake --build build/error_measuring
```

If you also would like to have debug info of every scanned point add the `DEBUG_SCANNED_POINTS=ON` option:

```bash
# Build the project with debug output with point debugging
cmake -S error_measuring -B build/error_measuring -D CMAKE_BUILD_TYPE=Debug -D DEBUG_SCANNED_POINTS=ON
cmake --build build/error_measuring
```

This will create the executable of the project into `build/error_measuring/`.

## Execution and parameters

_...comming soon..._