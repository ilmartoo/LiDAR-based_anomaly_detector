cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# ------------------------------ #
#             CONFIG             #
# ------------------------------ #

#
# Library root folder
#
set(LIB_FOLDER lib)
#
# Library subfolders
#
set(LIVOX_SDK_DIR ${LIB_FOLDER}/Livox-SDK-2.3.0/sdk_core)
set(READ_LVX_DIR ${LIB_FOLDER}/read_lvx-master)
set(ARMADILLO_DIR ${LIB_FOLDER}/armadillo-11.0.0)
set(CATCH2_DIR ${LIB_FOLDER})

# ------------------------------ #
#          PROJECT SETUP         #
# ------------------------------ #

#
# Project name
#
set(ANOMALY_PROJECT_NAME anomaly_detector)
project(${ANOMALY_PROJECT_NAME})
#
# Options
#
option(DEBUG_POINTS  "Debuggin option to print scanned points" OFF)  # Default: Do not print scanned points
option(CODE_COVERAGE "Enables coverage tests" OFF)                   # Default: Do not create coverage tests
#
# C++ standard
#
set(CMAKE_CXX_STANDARD 17)

# ------------------------------ #
#            LIBRARIES           #
# ------------------------------ #

#
# livox_sdk library
#
set(LIVOX_SDK_LIB livox_sdk)
project(${LIVOX_SDK_LIB})
add_subdirectory(${LIVOX_SDK_DIR})
target_link_libraries(${LIVOX_SDK_LIB}_static
	pthread
)
project(${ANOMALY_PROJECT_NAME})
link_directories(${LIVOX_SDK_DIR})
#
# read_lvx library
#
set(READ_LVX_LIB read_lvx)
add_library(${READ_LVX_LIB}
	SHARED
		${READ_LVX_DIR}/ldq.cpp
		${READ_LVX_DIR}/lds.cpp
		${READ_LVX_DIR}/lds_lvx.cpp
		${READ_LVX_DIR}/lvx_file.cpp
		${READ_LVX_DIR}/ldq.h
		${READ_LVX_DIR}/lds.h
		${READ_LVX_DIR}/lds_lvx.h
		${READ_LVX_DIR}/lvx_file.h
		${READ_LVX_DIR}/rapidxml/rapidxml.hpp
		${READ_LVX_DIR}/rapidxml/rapidxml_iterators.hpp
		${READ_LVX_DIR}/rapidxml/rapidxml_print.hpp
		${READ_LVX_DIR}/rapidxml/rapidxml_utils.hpp
)
target_link_libraries(${READ_LVX_LIB}
	${LIVOX_SDK_LIB}_static
	pthread
)
#
# armadillo library
#
add_subdirectory(${ARMADILLO_DIR})
link_directories(${ARMADILLO_DIR})


# ------------------------------ #
#           APPLICATION          #
# ------------------------------ #

#
# Application source code
#
file(GLOB_RECURSE sources CONFIGURE_DEPENDS
    src/*.cc
)
#
# Compilation & linking setup
#
string(TOUPPER "${CMAKE_BUILD_TYPE}" BUILD_TYPE)
set(CMAKE_CXX_FLAGS "-Wall -g3")
if(BUILD_TYPE STREQUAL "RELEASE")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -DNDEBUG")
endif()
if(BUILD_TYPE STREQUAL "DEBUG")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")
endif()
if(BUILD_TYPE STREQUAL "COVERAGE")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG")
endif()
if((BUILD_TYPE STREQUAL "DEBUG") OR (BUILD_TYPE STREQUAL "COVERAGE"))
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
endif()
if((BUILD_TYPE STREQUAL "COVERAGE") OR CODE_COVERAGE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage --coverage")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
	link_libraries(
		gcov
	)
endif()
#
# Include directories
#
include_directories(
	include/
	${LIVOX_SDK_DIR}/include
	${READ_LVX_DIR}/
	${ARMADILLO_DIR}/include
)
find_package(OpenMP REQUIRED)
link_libraries(
	${LIVOX_SDK_LIB}_static
	${READ_LVX_LIB}
	${ARMADILLO_LIB}
	lapack
	openblas
	OpenMP::OpenMP_CXX
	pthread
	m
)
#
# Application executable
#
if(NOT (BUILD_TYPE STREQUAL "COVERAGE"))
	add_executable(${ANOMALY_PROJECT_NAME}
		${sources}
	)
endif()

# ------------------------------ #
#           UNIT TESTS           #
# ------------------------------ #

#
# Enables testing
#
if((BUILD_TYPE STREQUAL "COVERAGE") OR CODE_COVERAGE)
	include(CTest)
	enable_testing()
	#
	# catch2 library
	#
	set(CATCH2_LIB catch2)
	add_library(${CATCH2_LIB} INTERFACE)
	target_include_directories( ${CATCH2_LIB}
		INTERFACE
			${CATCH2_DIR}
	)
	#
	# Unit tests source code
	#
	file(GLOB_RECURSE test_sources CONFIGURE_DEPENDS
		test/catch_main.cc
		test/catch_utils.hh
		test/*/*.cc
		src/*/*.cc
	)
	#
	# Linkers
	#
	include_directories(
		PUBLIC
			test/
			include/
	)
	link_libraries(
		${CATCH2_LIB}
	)
	#
	# Ejecutables
	#
	set(COVERAGE_TARGET coverage_tests)
	add_executable(${COVERAGE_TARGET}
		${test_sources}
	)
	#
	# Tests
	#
	add_test(
		NAME
			${COVERAGE_TARGET}
        COMMAND
			${COVERAGE_TARGET}
	)
	#
	# Ejecución de tests y generación de coverage report
	#
	add_custom_command( TARGET
							${COVERAGE_TARGET}
						COMMENT 
							"Run coverage tests"
						POST_BUILD 
						COMMAND 
							rm -f ./**/*.gcda && ctest ARGS -VV --progress --output-on-failure -j 4 || cat < /dev/null
						WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	)
	add_custom_command( TARGET
							${COVERAGE_TARGET}
						COMMENT 
							"Run coverage tests"
						POST_BUILD 
						COMMAND 
							lcov --capture --directory ${CMAKE_BINARY_DIR} --output-file lcov.info && genhtml lcov.info --output-directory coverage_report  || cat < /dev/null
						WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	)					
endif()
