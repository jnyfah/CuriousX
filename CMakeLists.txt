cmake_minimum_required (VERSION 3.8)

project ("CuriouX")

# Add source to this project's executable.
add_executable (CuriouX "main.cpp")

# TODO: Add tests and install targets if needed.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(Utils)

