# CMakeList.txt : CMake project for Kalculator, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("Kalculator")

# Add source to this project's executable.
add_executable (Kalculator "main.cpp")

# TODO: Add tests and install targets if needed.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

