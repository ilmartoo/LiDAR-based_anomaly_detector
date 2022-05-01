# Anomaly Detector

Using LiDAR data to detect any anomalies an object of a kind may have in comparison to a predefined model.

_...in-depth explanation to come..._

## Prerequisites

To compile the project you will need:

- `g++` version `9.4.0` or higher
- `cmake` version `14.0` or higher

We also need this libraries for the project to work:

- [livox-sdk 2.3.0](https://github.com/Livox-SDK/Livox-SDK/releases/tag/v2.3.0)
- [read_lvx](https://github.com/michalpelka/read_lvx)

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
./anomaly_detection/setup_libraries.sh
```

This script will create and install the needed libraries into their corresponding `lib/` subfolder, keeping the libraries from being installed into the `/usr/local/`.
### Project

To build de project execute the following from the **project root folder**:

```bash
# Build the project
cmake -S anomaly_detection -B build/anomaly_detection
cmake --build build/anomaly_detection
```

To build the project with debug output build it with `CMAKE_BUILD_TYPE` option set to `Debug`:

```bash
# Build the project with debug output
cmake -S anomaly_detection -B build/anomaly_detection -D CMAKE_BUILD_TYPE=Debug
cmake --build build/anomaly_detection
```

If you also would like to have debug info of every scanned point add the `DEBUG_SCANNED_POINTS=ON` option:

```bash
# Build the project with debug output with point debugging
cmake -S anomaly_detection -B build/anomaly_detection -D CMAKE_BUILD_TYPE=Debug -D DEBUG_SCANNED_POINTS=ON
cmake --build build/anomaly_detection
```

This will create the executable of the project into `build/anomaly_detection/`.

## Execution and parameters

_...comming soon..._


## Documentation

Further documentation of this project can be found on the `doxygen/` folder.

[Click here](doxygen/index.html) to take a peek.