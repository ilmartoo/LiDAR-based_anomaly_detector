# Lidar-based Anomaly Detector

Using LiDAR data to characterize objects using LiDAR-based point clouds and detect any anomalies objects of a kind may have in comparison to a predefined model.

---

## Prerequisites

To compile the project you will need:

- `gcc` version `9.4.0` or higher
- `cmake` version `14.0` or higher

We also need the following libraries for the project to work:

- [`livox-sdk v2.3.0`](https://github.com/Livox-SDK/Livox-SDK/releases/tag/v2.3.0)
- [`read_lvx`](https://github.com/michalpelka/read_lvx) (*Master branch, no version provided*)
- [`armadillo v11.0.0`](https://sourceforge.net/projects/arma/files/)
- [`catch2 v2.13.8`](https://github.com/catchorg/Catch2/releases/tag/v2.13.8) (*Only if you want to build and execute unit tests*)

---

## Developing enviroment

The project was developed and tested in a `Ubuntu 20.04.4` OS.

---

## Library instalation

The following libraries are needed for the project to compile:

- [`livox-sdk v2.3.0`](https://github.com/Livox-SDK/Livox-SDK/releases/tag/v2.3.0)
- [`read_lvx`](https://github.com/michalpelka/read_lvx) (*commit `b62b78da613fdb0c12cbde739916a992093ffac4` to master at 13/03/2022*)
- [`armadillo v11.0.0`](https://sourceforge.net/projects/arma/files/)

You can download and place them in the `lib/` folder like so:

```bash
wget https://github.com/Livox-SDK/Livox-SDK/archive/refs/tags/v2.3.0.zip && unzip v2.3.0.zip -d lib/ && rm v2.3.0.zip
wget https://github.com/michalpelka/read_lvx/archive/refs/heads/master.zip && unzip master.zip -d lib/ && rm master.zip
wget https://sourceforge.net/projects/arma/files/armadillo-11.0.0.tar.xz && tar -xf armadillo-11.0.0.tar.xz -C lib/ && rm armadillo-11.0.0.tar.xz
```
> One of `OpenBLAS` or `LAPACK` libraries are required for `armadillo` to have the needed functionalities. You may install them with your favourite package manager:
> 
> ```bash
> sudo apt install libopenblas-dev
> sudo apt install liblapack-dev
> ```

You are done! The `CMakeLists.txt` file will compile and link them to the project for you automaticaly.

> **Libraries for unit testing**
>
> If you want to compile and execute the unit tests you must download the `catch.hpp` from the [`catch2 v2.13.8`](https://github.com/catchorg/Catch2/releases/tag/v2.13.8) release and place it in the `lib/` folder:
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
cmake . -B build/Release -D CMAKE_BUILD_TYPE=Release
cmake --build build/Release
```

To build the project with debug output set the `CMAKE_BUILD_TYPE` option to `Debug`:

```bash
cmake . -B build/Debug -D CMAKE_BUILD_TYPE=Debug
cmake --build build/Debug
```

If you also would like to have debug info of every scanned point add the `DEBUG_SCANNED_POINTS=ON` option:

```bash
cmake . -B build/Debug -D CMAKE_BUILD_TYPE=Debug -D DEBUG_SCANNED_POINTS=ON
cmake --build build/Debug
```

> Any of this scripts will create the executable and install it into `build/<Config>`.

---

## Testing

To build the unit tests for the project:

```bash
cmake . -B build/Coverage -D CMAKE_BUILD_TYPE=Coverage -D BUILD_TESTS=ON
cmake --build build/Coverage
```

> This will build and install the `converage_tests` executable into the `build/Coverage` folder.

or 

```bash
cmake . -B build/<Config> -D CMAKE_BUILD_TYPE=<Config> -D CODE_COVERAGE=ON
cmake --build build/<Config>
```

> This will build and install the `converage_tests` executable into the `build/<Config>` folder.

---

## Execution and parameters

### Parameters

The program provides some parametes if you wish to lauch the application with the options specified by you:

```text
anomaly_detector <-b lidar_code | -f filename> [-t obj_frame_t] [-c chrono_mode] [-g back_frame_t] [-r reflectivity_threshold] [-d distance_threshold]
anomaly_detector <-h | --help>
```

Parameter explanation:

- `-b`: Broadcast code of the lidar sensor composed of 15 symbols maximum. **With 'default' as value it defaults to 3WEDH7600101621**

- `-f`: File with the 3D points to get the data from.

- `-t`: Miliseconds to use as frame duration time. **Defaults to `1500ms`.**

- `-c`: Type of chronometer to set up and measure time from. **Defaults to `notime`.**
  - `notime`: No chrono set
  - `char`: Characterizator chrono set
  - `anom`: Anomaly detector chrono set
  - `all`: All chronos set

- `-g`: Miliseconds during which scanned points will be part of the background. **Defaults to `5000ms`.**

- `-r`: Minimum reflectivity value points may have not to be discarded. **Defaults to `0 points`.**

- `-d`: Minimum distance from the background in meters a point must have not to be discarded. **Defaults to `0.01m`.**

- `-j`: Number of threads to execute the parallel regions with. **Defaults to `4 threads`.**

- `-h,--help`: Print the program help text.

**Any undefined options passed will be ignored.**

### Command Line Interface

The CLI has a variety of commands you can run:

- `help [command]`: Prints the help text of a command or all of them if not specified.

- `exit`: Exits the program.

- `chrono <set|unset> <...>`: Activation/Deactivation of specific chronometers.
  - `define`: Object and background definition routine.
  - `analyze`: Anomaly detection routine.
  - `all`: All of the above.

- `define <...>`: Definition and characterization of objects and background.
  - `background`: Defines the background.
  - `object [name]`: Defines an object with a specified name or an automatic generated one.

- `set <...>`: Modification of current execution parameters.
  - `backframe <millisecs>`: Milliseconds (integer) to scan for background points.
  - `objframe <millisecs>`: Milliseconds (integer) to scan for object points.
  - `backthreshold <meters>`: Meters (decimal) away an object point must be from the background to not be discarded.
  - `reflthreshold <points>`: Minimun reflectivity (decimal) a point must have to not be discarded.

- `discard <millisecs>`: Discards points for the amount of miliseconds specified.

- `object <...>`: Management of objects.
  - `describe <name>`: Describes the object with the given name.
  - `load <name> <file>`: Loads the contents of a file as a new object with the given name.
  - `save <name> <file>`: Saves the object with the given name into a file.
  - `csv <name> <file>`: Saves the object with the given name into a file in csv format.

- `model <...>`: Management of models.
  - `new <object> <new_model>`: Creates a new model from an object with the given name.
  - `describe <name>`: Describes the model with the given name.
  - `load <name> <file>`: Loads the contents of a file as a new model with the given name.
  - `save <name> <file>`: Saves the model with the given name into a file.
  - `csv <name> <file>`: Saves the model with the given name into a file in csv format.
- `info`: Prints the execution parameters currently in use.

- `list <...>`: List loaded/stored items.
  - `objects`: Created objects.
  - `models`: Loaded models.

- `analyze <object> <model>`: Analizes the diferences between the specified object and model.

---

## Documentation

Further documentation can be found inside `documentation.zip`. Decompress it to have access to the documentation folder:

```bash
unzip documentation.zip
```

Now, open the `index.html` file located in the `doxygen/` folder with your preferred browser to access the docs.
