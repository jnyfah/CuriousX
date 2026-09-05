cmake_minimum_required(VERSION 4.0)
project("CuriousX" VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS       "Build unit tests"          ON)
option(ENABLE_SANITIZERS "Build with ASan + UBSan"   OFF)
option(WARNINGS_AS_ERRORS "Treat warnings as errors" ON)

# -------------------- Warnings -----------------------------------------
add_library(curiousx_warnings INTERFACE)

if(MSVC)
    target_compile_options(curiousx_warnings INTERFACE /W4 /permissive-)
    if(WARNINGS_AS_ERRORS)
        target_compile_options(curiousx_warnings INTERFACE /WX)
    endif()
else()
    target_compile_options(curiousx_warnings INTERFACE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion 
        -Wold-style-cast
    )
    if(WARNINGS_AS_ERRORS)
        target_compile_options(curiousx_warnings INTERFACE -Werror)
    endif()
endif()

# -------------------- Sanitizers ---------------------------------------
add_library(curiousx_sanitizers INTERFACE)

if(ENABLE_SANITIZERS)
    if(MSVC)
        message(WARNING "ENABLE_SANITIZERS is not supported with MSVC; ignoring")
    else()
        target_compile_options(curiousx_sanitizers INTERFACE
            -fsanitize=address,undefined -fno-omit-frame-pointer -g)
        target_link_options(curiousx_sanitizers INTERFACE
            -fsanitize=address,undefined)
    endif()
endif()

# -------------------- Compiler -----------------------------------------
add_library(curiousx_core STATIC)

target_sources(curiousx_core
  PRIVATE
    src/lexer/lexer.cpp
    src/lexer/token.cpp
    src/parser/parser.cpp
    src/parser/arena.cpp
    src/sema/sema.cpp
)

target_include_directories(curiousx_core PUBLIC src)
target_link_libraries(curiousx_core
  PRIVATE curiousx_warnings
  PUBLIC  curiousx_sanitizers
)

add_executable(CuriousX src/main.cpp)
target_link_libraries(CuriousX PRIVATE curiousx_core curiousx_warnings)

# -------------------- Formatting ---------------------------------------
# Convenience targets only: formatting is enforced in CI, not by the build, so
# nobody without clang-format installed is blocked from compiling.
find_program(CLANG_FORMAT_EXE NAMES clang-format-18 clang-format)

if(CLANG_FORMAT_EXE)
    file(GLOB_RECURSE CX_FORMAT_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/src/*.cpp"   "${CMAKE_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_SOURCE_DIR}/tests/*.cpp" "${CMAKE_SOURCE_DIR}/tests/*.hpp")

    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXE} -i ${CX_FORMAT_SOURCES}
        COMMENT "Formatting sources with ${CLANG_FORMAT_EXE}"
        VERBATIM)

    add_custom_target(format-check
        COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror ${CX_FORMAT_SOURCES}
        COMMENT "Checking formatting with ${CLANG_FORMAT_EXE}"
        VERBATIM)
endif()

# -------------------- Tests --------------------------------------------
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
