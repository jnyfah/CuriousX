include_directories("${PROJECT_SOURCE_DIR}")

FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(
    lexerTest
    lexerTest.cpp
)


target_link_libraries(
  lexerTest
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(lexerTest)