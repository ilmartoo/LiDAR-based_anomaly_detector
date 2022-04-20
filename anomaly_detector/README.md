# Anomaly Detector

Using LiDAR data to detect any anomalies an object of a kind may have in comparison to a predefined model.

_...in-depth explanation to come..._

## Prerequisites

To compile the project you will need:

- `g++` version `10.3.0` or higher [^1]
- `cmake` version `14.0` or higher

[^1]: May work with older versions, but has not been tested.

## Testing enviroment

The project was developed and tested in `Ubuntu 21.04` [^2].

[^2]: Should work with almost every GNU/Linux distribution. Feel free to test it yourself ;)

## Setup

### Libraries

To setup the necesary libraries you can use the script `setup_libraries.sh` [^3] from the **root folder** just like so:

```bash
# Setup needed libraries
./setup_libraries.sh
```

If you want to execute the created tests, do:

```bash
# Setup needed libraries + test libraries
./setup_libraries.sh -t
```

[^3]: This script will create and install the libraries into their corresponding `lib/` subfolder, keeping the libraries from being installed into the `/usr/local/` folder and forgotten no long after (just like we always do).

### Project

To build de project execute the following from the **`anomaly_detector/` folder**: 

```bash
# Build the project
cmake -S . -B build/Release -D CMAKE_BUILD_TYPE=Release
cmake --build build/Release
```

To build the project with debug output execute the following from the **`anomaly_detector/` folder**:

```bash
# Build the project with debug output
cmake -S . -B build/Debug -D CMAKE_BUILD_TYPE=Debug
cmake --build build/Debug
```

If you would like to have debug info of every scanned point, execute the following instead:

```bash
# Build the project with debug output with point debugging
cmake -S . -B build/Debug -D CMAKE_BUILD_TYPE=Debug -D DEBUG_POINTS=ON
cmake --build build/Debug
```

This will create the executable of the project into the corresponding `build/<cmake_build_type>/` subfolder.

## Execution and parameters

_...comming soon..._
