# cmake version
cmake_minimum_required(VERSION 2.8)
# project
project(ccmd)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-std=c++11 -g -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-std=c++11 -g -O2")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE release)
endif()
if (${CMAKE_BUILD_TYPE} STREQUAL debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

# build cflag
add_subdirectory(third/cflag)

# add resources
aux_source_directory(./src ccmd_srcs)

# include directory
include_directories(./include ./third/cflag/include)

# static library
add_library(ccmd STATIC "${ccmd_srcs}")

# build example
# add resources
aux_source_directory(./example example_srcs)

# build example
add_executable(ccmd-example "${example_srcs}")

#link library
target_link_libraries(ccmd-example ccmd cflag)

# build test
if (${CMAKE_BUILD_TYPE} STREQUAL debug)
    add_subdirectory(tests)
endif()
