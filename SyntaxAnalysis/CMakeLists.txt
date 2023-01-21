project (SyntaxAnalysis)

add_library(${PROJECT_NAME} src/Parser.cpp)

target_include_directories(${PROJECT_NAME}
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
        $<INSTALL_INTERFACE:include>                                # in case we will install our lib 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>      # when we use lib - where to look up public headers
        ${CMAKE_CURRENT_SOURCE_DIR}/src                             # private headers - only for build lib
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)


