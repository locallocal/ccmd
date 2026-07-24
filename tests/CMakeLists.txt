add_executable(
    ccmd-test
    test_ccmd_bool.cc
    test_ccmd_common.cc
    test_ccmd_float.cc
    test_ccmd_int.cc
    test_ccmd_string.cc
)
target_link_libraries(ccmd-test PRIVATE ccmd::ccmd GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(ccmd-test)
