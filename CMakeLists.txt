# Cmake config
cmake_minimum_required(VERSION 3.13)

set(CMAKE_INSTALL_PREFIX ".")


# Proyect config
set(MY_PROYECT "anomaly_detector")
project(${MY_PROYECT})


# Add library
add_library(sdk_core "")


# Add subdirectories
add_subdirectory(src)
add_subdirectory(include)


# Set executable name
add_executable(${MY_PROYECT} "")


# Build destination
install(TARGETS ${MY_PROYECT} DESTINATION bin)