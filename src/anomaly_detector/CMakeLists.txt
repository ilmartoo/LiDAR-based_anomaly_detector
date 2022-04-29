cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# Project name
project(anomaly_detection)

# Options
option(DEBUG_POINTS "Opción de debugging para imprimir los puntos escaneados" OFF)

# Release flags
set(CMAKE_CXX_FLAGS_RELEASE "-O1 -DNDEBUG") 

# Debug flags
if(DEBUG_SCANNED_POINTS)
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -DDEBUG_POINTS")
else()
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -g3")

# Root
set(PROJECT_ROOT "../../")

# Gather all source files
file(GLOB_RECURSE sources CONFIGURE_DEPENDS ${PROJECT_ROOT}/src/anomaly_detection/*.cc)

# Include directories
include_directories(
  ${PROJECT_ROOT}/include/
)

##### Libraries

# SDK CORE
set(SDKCORE_LIB ${PROJECT_ROOT}/lib/sdk_core/lib/liblivox_sdk_static.a)
include_directories(${PROJECT_ROOT}/lib/sdk_core/include)

# READ LVX
set(READLVX_LIB ${PROJECT_ROOT}/lib/read_lvx/lib/libread_lvx.a)
include_directories(${PROJECT_ROOT}/lib/read_lvx)


# Executable
add_executable(${PROJECT_NAME} ${sources})

# Linking libraries
target_link_libraries(
  ${PROJECT_NAME}
  ${SDKCORE_LIB}  # sdk_core
  ${READLVX_LIB}  # read_lvx
  m               # mathematical library
  pthread         # thread library
)
