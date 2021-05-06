add_executable(cslibs_test
    DefsFileTest.cpp
    DiscDbTest.cpp
    EffectDbTest.cpp
    GelDbTest.cpp
    GoboDbTest.cpp
    )

target_link_libraries(cslibs_test PRIVATE cslibs GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(cslibs_test)
