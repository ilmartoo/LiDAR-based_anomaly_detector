cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

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
option(DEBUG_POINTS "Debuggin option to print scanned points" OFF) # Default: Do not print scanned points
option(BUILD_TESTS "Enables unit tests" OFF)                       # Default: Do not create unit tests
#
# C++ standard
#
set(CMAKE_CXX_STANDARD 17)

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
# Compilation setup
#
set(CMAKE_CXX_FLAGS "-Wall -g3")            # Compiler flags
string(TOLOWER ${CMAKE_BUILD_TYPE} build_type)
if(NOT build_type STREQUAL "debug" AND NOT build_type STREQUAL "release")
	message(FATAL_ERROR "Unknown build type")
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG") # Release compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG") 		# Debug compiler flags
if(DEBUG_SCANNED_POINTS)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_POINTS") # Debug extra compiler flags
endif()
#
# Application source code
#
file(GLOB_RECURSE sources CONFIGURE_DEPENDS
    src/*.cc
)
#
# Include directories
#
include_directories(
	include/
	${LIVOX_SDK_DIR}/include
	${READ_LVX_DIR}/
	${ARMADILLO_DIR}/include
)
link_libraries(
	${LIVOX_SDK_LIB}_static
	${READ_LVX_LIB}
	${ARMADILLO_LIB}
	pthread
	m
)
#
# Application executable
#
add_executable(${ANOMALY_PROJECT_NAME}
	${sources}
)

# ------------------------------ #
#           UNIT TESTS           #
# ------------------------------ #

#
# Enables testing
#
if(BUILD_TESTS AND build_type STREQUAL "release")
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
	file(GLOB_RECURSE unit_test_sources CONFIGURE_DEPENDS
		test/*.cc
	)
	#
	# Unit tests executable
	#
	set(UNIT_TESTS_NAME unit_tests)
	add_executable(${UNIT_TESTS_NAME}
		${unit_test_sources}
	)
	target_include_directories(${UNIT_TESTS_NAME}
		PUBLIC
			test/
	)
	target_link_libraries(${UNIT_TESTS_NAME}
		${CATCH2_LIB}
	)
	add_test(
		NAME
			${UNIT_TESTS_NAME}
        COMMAND
			${UNIT_TESTS_NAME}
	)
endif()
