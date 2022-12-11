include_directories("${PROJECT_SOURCE_DIR}")

FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(
    curiousTest
    curiousTest.cpp
)


target_link_libraries(
  curiousTest
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(curiousTest)