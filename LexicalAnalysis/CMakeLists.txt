project (LexicalAnalysis)
include_directories(Utils)

add_library(${PROJECT_NAME} include/Lexer.hpp src/LexerToken.cpp)

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_sources( ${PROJECT_NAME}
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
        $<INSTALL_INTERFACE:include>                                # in case we will install our lib 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> )