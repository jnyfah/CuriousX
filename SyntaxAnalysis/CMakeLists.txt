project (SyntaxAnalysis)

add_library(${PROJECT_NAME} src/Parser.cpp)

include_directories(Utils)
include_directories(LexicalAnalysis)

target_link_libraries(${PROJECT_NAME} PUBLIC LexicalAnalysis)


target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_sources( ${PROJECT_NAME}
PUBLIC ${PROJECT_SOURCE_DIR}/src)

