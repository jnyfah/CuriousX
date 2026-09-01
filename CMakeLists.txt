cmake_minimum_required(VERSION 4.0)
project("CuriousX" VERSION 2.0)

# -------------------- Set C++ Standard --------------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(curiousx_core STATIC)

target_sources(curiousx_core
  PRIVATE
    src/lexer/lexer.cpp
    src/lexer/token.cpp
    src/parser/parser.cpp
    src/parser/arena.cpp
)

target_include_directories(curiousx_core PUBLIC src)

add_executable(CuriousX
  src/main.cpp
)

target_link_libraries(CuriousX
  PRIVATE curiousx_core
)

enable_testing()
add_subdirectory(tests)

# option(BUILD_TESTS "Build unit tests" OFF)
# if(BUILD_TESTS)
#     add_subdirectory(tests)
# endif()

# option(ENABLE_SANITIZERS "Build with ASan + UBSan" OFF)
# if(ENABLE_SANITIZERS)
#     add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -g)
#     add_link_options(-fsanitize=address,undefined)
# endif()
