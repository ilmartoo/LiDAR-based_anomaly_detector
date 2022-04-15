cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# Project name
project(anomaly_detection)

set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG_LBAD")   # Debug flags
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG") # Release flags

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -g3")

# Gather all source files
file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/main/*.cc)

# Include directories
include_directories(
  ${CMAKE_SOURCE_DIR}/include/
)

# Libraries

# SDK CORE
set(SDKCORE_LIB ${CMAKE_SOURCE_DIR}/lib/sdk_core/lib/liblivox_sdk_static.a)
include_directories(${CMAKE_SOURCE_DIR}/lib/sdk_core/lib/include)

# READ LVX
set(READLVX_LIB ${CMAKE_SOURCE_DIR}/lib/read_lvx/lib/libread_lvx.a)
include_directories(${CMAKE_SOURCE_DIR}/lib/read_lvx/lib/include)


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
