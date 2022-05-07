cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)

# Project name
project(error_measuring)

# Options
option(CMAKE_BUILD_TYPE "Tipo de build" Release)
option(DEBUG_POINTS "Opción de debugging para imprimir los puntos escaneados" OFF)

# Release flags
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG") 

# Debug flags
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")
if(DEBUG_SCANNED_POINTS)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_POINTS")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -g3")

# Root
set(PROJECT_ROOT "${PROJECT_SOURCE_DIR}/..")

# Gather all source files
file(GLOB_RECURSE sources CONFIGURE_DEPENDS 
  ${PROJECT_ROOT}/error_measuring/src/*.cc
  ${PROJECT_ROOT}/anomaly_detector/src/scanner/ScannerLVX.cc
  ${PROJECT_ROOT}/anomaly_detector/src/scanner/ScannerCSV.cc
)

# Include directories
include_directories(
  ${PROJECT_ROOT}/include/      # Project includes
  ${PROJECT_ROOT}/libs/include  # Library includes
)

## Libraries
# SDK CORE
set(SDKCORE_LIB ${PROJECT_ROOT}/libs/lib/liblivox_sdk.a)
# READ LVX
set(READLVX_LIB ${PROJECT_ROOT}/libs/lib/libread_lvx.a)
# ARMADILLO
set(ARMADILLO_LIB ${PROJECT_ROOT}/libs/lib/libarmadillo.so)
# OPENBLAS
set(OPENBLAS_LIB openblas)


# Executable
add_executable(${PROJECT_NAME} ${sources})

# Linking libraries
target_link_libraries(
  ${PROJECT_NAME}
  ${SDKCORE_LIB}    # sdk_core
  ${READLVX_LIB}    # read_lvx
  ${OPENBLAS_LIB}   # openblas
  ${ARMADILLO_LIB}  # armadillo
  m                 # mathematical library
  pthread           # thread library
)
