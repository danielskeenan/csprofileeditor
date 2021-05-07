add_executable(csprofile_test
    LibraryTest.cpp
    ParameterTest.cpp
    RangeTest.cpp
    )
target_link_libraries(csprofile_test PRIVATE csprofile GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(csprofile_test)
