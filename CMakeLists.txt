cmake_minimum_required(VERSION 3.25)
project("CuriousX" VERSION 2.0)

# -------------------- Set C++ Standard --------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -------------------- Set Compiler Warnings --------------------------------
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

#add -Werror and /WX

# -------------------- Fetch Json component --------------------------------
include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

# -------------------- Specify the source files for each component --------------------------------
set(SOURCES_LEXER
    CuriousX/Lexer/LexerToken.cpp
)

set(SOURCES_PARSER
    CuriousX/Parser/Parser.cpp
)

set(SOURCES_SEMANTIC
    CuriousX/Semantic/Semantic.cpp
)

set(SOURCES_GEN
    CuriousX/Gen/Codegen.cpp
)


#------------ Include Utility Folder--------------------------------
add_library(Utils INTERFACE)
target_include_directories(Utils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Utils)

# -------------------- Include Lexer Folder--------------------------------
add_library(Lexer STATIC ${SOURCES_LEXER})
target_include_directories(Lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/CuriousX/Lexer)
target_link_libraries(Lexer PUBLIC Utils nlohmann_json::nlohmann_json)

# -------------------- Include Parser Folder--------------------------------
add_library(Parser STATIC ${SOURCES_PARSER})
target_include_directories(Parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/CuriousX/Parser)
target_link_libraries(Parser PUBLIC Lexer)

# -------------------- Include Semantic Folder--------------------------------
add_library(Semantic STATIC ${SOURCES_SEMANTIC})
target_include_directories(Semantic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/CuriousX/Semantic)
target_link_libraries(Semantic PUBLIC Parser)

# -------------------- Include CodeGen Folder--------------------------------
add_library(Gen STATIC ${SOURCES_GEN})
target_include_directories(Gen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/CuriousX/Gen)
target_link_libraries(Gen PUBLIC Semantic)

# -------------------- Create the main executable and link it to the libraries --------------------------------
add_executable(CuriousX "CuriousX/main.cpp")
target_link_libraries(CuriousX PRIVATE Gen)
target_include_directories(CuriousX PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/CuriousX)


if(EMSCRIPTEN)
    set_target_properties(CuriousX PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/web"  
        LINK_FLAGS "--bind" 
    )
endif()