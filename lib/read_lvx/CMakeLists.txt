cmake_minimum_required(VERSION 3.14)

project(read_lvx) # Project name

add_library(${PROJECT_NAME} STATIC "")

target_link_libraries(${PROJECT_NAME} pthread)

target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/../sdk_core/lib/liblivox_sdk_static.a)

include_directories(${PROJECT_SOURCE_DIR}/../sdk_core/include)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "ldq.h;lds.h;lds_lvx.h;lvx_file.h")

target_sources(
  ${PROJECT_NAME}
  PRIVATE
  ldq.cpp
  lds.cpp
  lds_lvx.cpp
  lvx_file.cpp
  ldq.h
  lds.h
  lds_lvx.h
  lvx_file.h
  rapidxml/rapidxml.hpp
  rapidxml/rapidxml_iterators.hpp
  rapidxml/rapidxml_print.hpp
  rapidxml/rapidxml_utils.hpp
)

install(
  TARGETS ${PROJECT_NAME}
  PUBLIC_HEADER DESTINATION ${CMAKE_SOURCE_DIR}/lib/include/${PROJECT_NAME}
  ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/lib
  LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/lib
)

install(
  FILES
  rapidxml/rapidxml.hpp
  rapidxml/rapidxml_iterators.hpp
  rapidxml/rapidxml_print.hpp
  rapidxml/rapidxml_utils.hpp
  DESTINATION
  ${CMAKE_SOURCE_DIR}/lib/include/rapidxml
)
