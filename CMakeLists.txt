cmake_minimum_required(VERSION 3.25)
project("CuriousX" VERSION 2.0)

# -------------------- Set C++ Standard --------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -------------------- Compiler Warnings --------------------------------
if(NOT EMSCRIPTEN)
    if(MSVC)
        add_compile_options(/W4 /WX /MP)
        add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    else()
        add_compile_options(
            -Wall -Wextra -Wpedantic -Werror
            -Wno-unused-parameter
            -Wno-missing-field-initializers
        )
    endif()
endif()

# -------------------- FetchContent Setup --------------------------------
include(FetchContent)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG master
)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
    FetchContent_MakeAvailable(json)
endif()

# -------------------- Build Tests Option --------------------------------
option(BUILD_TESTS "Build tests" OFF)
message(STATUS "[STX] Build tests: ${BUILD_TESTS}")

# -------------------- Include Utility Folder --------------------------------
add_library(CompilerUtils INTERFACE)
target_include_directories(CompilerUtils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/CompilerUtils)

# -------------------- Include Lexer --------------------------------
set(SOURCES_LEXER CuriousX/Lexer/LexerToken.cpp)
add_library(Lexer STATIC ${SOURCES_LEXER})
target_include_directories(Lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/CuriousX/Lexer)
target_link_libraries(Lexer PUBLIC CompilerUtils nlohmann_json::nlohmann_json)

# -------------------- Include Parser --------------------------------
set(SOURCES_PARSER CuriousX/Parser/Parser.cpp)
add_library(Parser STATIC ${SOURCES_PARSER})
target_include_directories(Parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/CuriousX/Parser)
target_link_libraries(Parser PUBLIC Lexer)

# -------------------- Include Semantic Analyzer --------------------------------
set(SOURCES_SEMANTIC CuriousX/Semantic/Semantic.cpp)
add_library(Semantic STATIC ${SOURCES_SEMANTIC})
target_include_directories(Semantic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/CuriousX/Semantic)
target_link_libraries(Semantic PUBLIC Parser)

# -------------------- Include Code Generator --------------------------------
set(SOURCES_GEN CuriousX/Generation/Codegen.cpp)
add_library(Gen STATIC ${SOURCES_GEN})
target_include_directories(Gen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/CuriousX/Generation)
target_link_libraries(Gen PUBLIC Semantic)

# -------------------- Include Compiler Core --------------------------------
add_library(Compiler STATIC CuriousX/Compiler.cpp)
target_include_directories(Compiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/CuriousX)
target_link_libraries(Compiler PUBLIC Parser Semantic Gen)

# -------------------- Main Executable --------------------------------
add_executable(CuriousX CuriousX/main.cpp)
target_include_directories(CuriousX PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/CuriousX)
target_link_libraries(CuriousX PRIVATE Compiler)

# -------------------- Tests --------------------------------
if(BUILD_TESTS)
    FetchContent_Declare(
        gtest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG main
    )

    FetchContent_GetProperties(gtest)
    if(NOT gtest_POPULATED)
        FetchContent_MakeAvailable(gtest)
    endif()

    include(CTest)
    enable_testing()

    file(GLOB SOURCES_tests tests/*.cpp)
    add_executable(curiousx_tests ${SOURCES_tests})
    target_include_directories(curiousx_tests PRIVATE ${CMAKE_SOURCE_DIR}/CuriousX ${CMAKE_SOURCE_DIR}/CompilerUtils)
    target_link_libraries(curiousx_tests PRIVATE GTest::gtest GTest::gtest_main Compiler)
    add_test(NAME CuriousxTests COMMAND curiousx_tests)
endif()

# -------------------- Emscripten Support --------------------------------
if(EMSCRIPTEN)
    set_target_properties(CuriousX PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/CompilerEditor"
    )
    target_link_options(CuriousX PRIVATE --bind -s DISABLE_EXCEPTION_CATCHING=0)
    message(STATUS "Building with Emscripten support")
endif()
