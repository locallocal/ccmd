cmake_minimum_required(VERSION 3.16)

project(
    ccmd
    VERSION 0.0.1
    DESCRIPTION "A small command-line parser for C++11 and later"
    LANGUAGES CXX
)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(CCMD_TOP_LEVEL ON)
else()
    set(CCMD_TOP_LEVEL OFF)
endif()

option(CCMD_BUILD_EXAMPLES "Build the ccmd example application" ${CCMD_TOP_LEVEL})
option(CCMD_BUILD_TESTS "Build the ccmd test suite" OFF)
option(CCMD_FETCH_TEST_DEPS "Download GoogleTest when it is not installed" ON)
option(CCMD_ENABLE_COVERAGE "Enable GCC/Clang coverage instrumentation" OFF)

add_library(
    ccmd_cflag
    STATIC
    third/cflag/src/bool.cc
    third/cflag/src/cflag.cc
    third/cflag/src/float.cc
    third/cflag/src/int.cc
    third/cflag/src/string.cc
)
add_library(ccmd::cflag ALIAS ccmd_cflag)
set_target_properties(ccmd_cflag PROPERTIES EXPORT_NAME cflag OUTPUT_NAME cflag)
target_compile_features(ccmd_cflag PUBLIC cxx_std_11)
target_include_directories(
    ccmd_cflag
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third/cflag/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

add_library(
    ccmd
    STATIC
    src/bool.cc
    src/ccmd.cc
    src/float.cc
    src/int.cc
    src/string.cc
)
add_library(ccmd::ccmd ALIAS ccmd)
target_compile_features(ccmd PUBLIC cxx_std_11)
target_include_directories(
    ccmd
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(ccmd PUBLIC ccmd::cflag)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(ccmd PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(CCMD_ENABLE_COVERAGE)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(FATAL_ERROR "CCMD_ENABLE_COVERAGE requires GCC or Clang")
    endif()
    foreach(target ccmd_cflag ccmd)
        target_compile_options(${target} PRIVATE --coverage -O0 -g)
        target_link_options(${target} PUBLIC --coverage)
    endforeach()
endif()

if(CCMD_BUILD_EXAMPLES)
    add_executable(ccmd-example example/example.cc)
    target_link_libraries(ccmd-example PRIVATE ccmd::ccmd)
endif()

if(CCMD_BUILD_TESTS)
    enable_testing()
    find_package(GTest QUIET)
    if(NOT GTest_FOUND)
        if(NOT CCMD_FETCH_TEST_DEPS)
            message(FATAL_ERROR
                "GoogleTest was not found. Install it or configure with CCMD_FETCH_TEST_DEPS=ON.")
        endif()
        include(FetchContent)
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.tar.gz
            URL_HASH SHA256=81964fe578e9bd7c94dfdb09c8e4d6e6759e19967e397dbea48d1c10e45d0df2
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
        set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif()
    add_subdirectory(tests)
endif()

install(
    TARGETS ccmd ccmd_cflag
    EXPORT ccmdTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(
    FILES
        include/ccmd.h
        third/cflag/include/cflag.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(
    EXPORT ccmdTargets
    FILE ccmdTargets.cmake
    NAMESPACE ccmd::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ccmd
)

configure_package_config_file(
    cmake/ccmdConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ccmdConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ccmd
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/ccmdConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/ccmdConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/ccmdConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ccmd
)
