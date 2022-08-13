project (LexicalAnalysis)
include_directories(Utils)

add_library(${PROJECT_NAME} src/LexerToken.cpp)

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_sources( ${PROJECT_NAME}
PUBLIC ${PROJECT_SOURCE_DIR}/src)