cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(lidar-based_anomaly_detector)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(anomaly_detection)
add_subdirectory(error_measuring)