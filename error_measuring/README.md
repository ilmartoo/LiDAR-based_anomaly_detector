# Error Measuring

Project to measure the cuadratic error a predefined LiDAR data may have using plane computation.

*...in-depth explanation to come...*

---

## Prerequisites

To compile the project you will need:

- `g++` version `9.4.0` or higher
- `cmake` version `14.0` or higher

We also need the following libraries for the project to work:

- [`livox-sdk 2.3.0`](https://github.com/Livox-SDK/Livox-SDK/releases/tag/v2.3.0)
- [`read_lvx`](https://github.com/michalpelka/read_lvx)
- [`armadillo 10.0.0`](https://sourceforge.net/projects/arma/files/)

---

## Developing enviroment

The project was developed and tested in `Ubuntu 20.04.4` and `Ubuntu 21.04`.

---

## Library compilation

### Quick installation

In the `libraries.zip` are located **minimalist versions of the needed libraries with some modifications** so they can installed without having to modify the libraries any further.

To easily install the libraries:

```bash
unzip libraries.zip -d bin/
./error_measuring/setup_libraries.sh
```

> OpenBLAS (or LAPACK) library needs to be installed for `armadillo` to work properly, so you may be prompt with a request to install the libopenblas-dev package if not present in your system. If you do not wish to install it through the script you may install with your preferred package manager or manually (be sure to link it properly to `armadillo` if you wish to do it this way).

This will decompress the minimalist libraries to the `bin/` folder, compile the libraries and install them in the `libs/` folder, without the need to modify the project's `CMakeLists.txt` file.

### Manualy installation

To manualy install the needed libraries you may download the specified version of the libraries:

- [`livox-sdk 2.3.0`](https://github.com/Livox-SDK/Livox-SDK/releases/tag/v2.3.0)
- [`read_lvx`](https://github.com/michalpelka/read_lvx) (First version, no version number specified)
- [`armadillo 10.0.0`](https://sourceforge.net/projects/arma/files/)

***Note:*** *As we said in [Quick Instalation](#quick-installation), `armadillo` depends on OpenBLAS (or LAPACK) library. Be sure to install the library development package beforehand.*

Then, compile the libraries as stipulated by their corresponding documentation.

> The `read_lvx` project is not a library, but can be compiled as so. It also needs `livox_sdk` library to work, so be aware of it when compiling.

When all the needed libraries are compiled, be sure to change the needed lines in the [`CMakeLists.txt`](CMakeLists.txt) file:

```cmake
# Include directories
include_directories(
  ${PROJECT_ROOT}/include/      # Project includes
  ${PROJECT_ROOT}/libs/include  # Library includes          <------ CHANGE THIS to the location of the libraries' includes
)

## Libraries
# SDK CORE
set(SDKCORE_LIB ${PROJECT_ROOT}/libs/lib/liblivox_sdk.a)    <------ CHANGE THIS to the location of the compiled livox_sdk lib
# READ LVX
set(READLVX_LIB ${PROJECT_ROOT}/libs/lib/libread_lvx.a)     <------ CHANGE THIS to the location of the compiled read_lvx lib
# ARMADILLO
set(ARMADILLO_LIB ${PROJECT_ROOT}/libs/lib/libarmadillo.so) <------ CHANGE THIS to the location of the compiled armadillo lib
# OPENBLAS
set(OPENBLAS_LIB openblas)                                  <------ CHANGE THIS to the location of the compiled OpenBLAS lib
                                                            (leave it like this if you installed it through the package manager)
```

If you did all the steps correctly, you are now ready to compile the project.

---

## Project compilation

To build de project execute the following from the **project root folder**:

```bash
cmake -S error_measuring -B build/error_measuring
cmake --build build/error_measuring
```

To build the project with debug output build it with `CMAKE_BUILD_TYPE` option set to `Debug`:

```bash
cmake -S error_measuring -B build/error_measuring -D CMAKE_BUILD_TYPE=Debug
cmake --build build/error_measuring
```

If you also would like to have debug info of every scanned point add the `DEBUG_SCANNED_POINTS=ON` option:

```bash
cmake -S error_measuring -B build/error_measuring -D CMAKE_BUILD_TYPE=Debug -D DEBUG_SCANNED_POINTS=ON
cmake --build build/error_measuring
```

This will create the executable of the project into `build/error_measuring/`.

---

## Execution and parameters

*...comming soon...*
