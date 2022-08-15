include_directories("${PROJECT_SOURCE_DIR}")

# First lexerTest Google Test
add_executable(lexerTest lexerTest.cpp)
target_link_libraries(lexerTest gtest gmock_main)
add_test(NAME lexerTest_test COMMAND lexerTest)