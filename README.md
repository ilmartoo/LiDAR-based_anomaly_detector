# Lidar-based Anomaly Detector

Using LiDAR data to detect any anomalies an object of a kind may have in comparison to a predefined model.

*...in-depth explanation to come...*

---

## Prerequisites

To compile the project you will need:

- `g++` version `9.4.0` or higher
- `cmake` version `14.0` or higher

We also need the following libraries for the project to work:

- [`livox-sdk 2.3.0`](https://github.com/Livox-SDK/Livox-SDK/releases/tag/v2.3.0)
- [`read_lvx`](https://github.com/michalpelka/read_lvx) (*Master branch, no version provided*)
- [`armadillo 10.0.0`](https://sourceforge.net/projects/arma/files/)
- [`Catch 2.13.8`](https://github.com/catchorg/Catch2/releases/tag/v2.13.8) (*Only if you want to build and execute unit tests*)

---

## Developing enviroment

The project was developed and tested in `Ubuntu 20.04.4` and `Ubuntu 21.04`.

---

## Library instalation

The following libraries are needed for the project to compile:

- [`livox-sdk 2.3.0`](https://github.com/Livox-SDK/Livox-SDK/releases/tag/v2.3.0)
- [`read_lvx`](https://github.com/michalpelka/read_lvx) (*Master branch, no version provided*)
- [`armadillo 10.0.0`](https://sourceforge.net/projects/arma/files/)

You can download and place them in the `lib/` folder like so:

```bash
wget https://github.com/Livox-SDK/Livox-SDK/archive/refs/tags/v2.3.0.zip && unzip Livox-SDK-2.3.0.zip -d lib/ && rm Livox-SDK-2.3.0.zip
wget https://github.com/michalpelka/read_lvx/archive/refs/heads/master.zip && unzip read_lvx-master.zip -d lib/ && rm read_lvx-master.zip
wget https://sourceforge.net/projects/arma/files/armadillo-11.0.0.tar.xz && tar -xf armadillo-11.0.0.tar.xz -C lib/ && rm armadillo-11.0.0.tar.xz
```
> One of `OpenBLAS` or `LAPACK` libraries are required for `armadillo-11.0.0` to have the needed functionalities. You may install them with your favourite package manager:
> 
> ```bash
> sudo apt install libopenblas-dev liblapack-dev
> ```

You are done! The `CMakeLists.txt` file will compile and link them to the project for you automaticaly.

> **Libraries for unit testing**
>
> If you want to compile and execute the unit tests you must download the `catch.hpp` from the [`Catch 2.13.8`](https://github.com/catchorg/Catch2/releases/tag/v2.13.8) release and place it in the `lib/` folder:
>
> ```bash
> wget https://github.com/catchorg/Catch2/releases/download/v2.13.8/catch.hpp && mv catch.hpp lib/
> ```

> **Library folder modification**
>
> In the case you wish to change the root location for the libraries or rename any library folder, you may edit the [`CMakeLists.txt`](CMakeLists.txt) file according to your modifications:
>
> ```cmake
> # ------------------------------ #
> #             CONFIG             #
> # ------------------------------ #
> 
> #
> # Library root folder
> #
> set(LIB_FOLDER lib) # <------------------ LIBRARY ROOT FOLDER
> #
> # Library subfolders
> #
> set(LIVOX_SDK_DIR ${LIB_FOLDER}/Livox-SDK-2.3.0/sdk_core) # <---- LIVOX_SDK FOLDER
> set(READ_LVX_DIR ${LIB_FOLDER}/read_lvx-master) # <-------------- READ_LVX FOLDER
> set(ARMADILLO_DIR ${LIB_FOLDER}/armadillo-11.0.0) # <------------ ARMADILLO FOLDER
> set(CATCH2_DIR ${LIB_FOLDER}) # <-------------------------------- CATCH2 FOLDER
> ```

---

## Project compilation

To build de project execute:

```bash
cmake . -B build/ -D CMAKE_BUILD_TYPE=Release
cmake --build build
```

To build the project with debug output build it with `CMAKE_BUILD_TYPE` option set to `Debug`:

```bash
cmake . -B build/ -D CMAKE_BUILD_TYPE=Debug
cmake --build build
```

If you also would like to have debug info of every scanned point add the `DEBUG_SCANNED_POINTS=ON` option:

```bash
cmake . -B build/ -D CMAKE_BUILD_TYPE=Debug -D DEBUG_SCANNED_POINTS=ON
cmake --build build
```

Any of this scripts will create the executable and install it into `build/<build_type>/`.

---

## Testing

To build the unit tests for the project:

```bash
cmake . -B build/ -D CMAKE_BUILD_TYPE=Release -D BUILD_TESTS=ON
cmake --build build
```

This will build and install the `unit_tests` executable into the `build/Testing/` folder.

---

## Execution and parameters

*...comming soon...*

---

## Documentation

Further documentation can be found inside `documentation.zip`. Decompress it to have access to the documentation folder:

```bash
unzip documentation.zip
```

Nos, open the `index.html` file located in the `doxygen/` folder with your preferred browser to access the docs.
