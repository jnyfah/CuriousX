cmake_minimum_required (VERSION 3.8)

project ("CuriousX")


# Add source to this project's executable.
add_executable (CuriousX "main.cpp")

# TODO: Add tests and install targets if needed.
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
else()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif()


set(CMAKE_CXX_EXTENSIONS OFF)


enable_testing()


include_directories(Utils)
include_directories(tests)
add_subdirectory(LexicalAnalysis)

target_link_libraries( ${PROJECT_NAME} LexicalAnalysis)



