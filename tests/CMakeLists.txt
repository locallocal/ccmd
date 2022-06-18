# include(FetchContent)
# FetchContent_Declare(
#     googletest
#     URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
#)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# add resources
aux_source_directory(. test_srcs)

# enable test
enable_testing()

# build test
add_executable(ccmd-test ${test_srcs})

# link library
target_link_libraries(ccmd-test ccmd cflag gtest gtest_main pthread)

include(GoogleTest)
gtest_discover_tests(ccmd-test)
