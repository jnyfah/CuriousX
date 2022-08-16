cmake_minimum_required (VERSION 3.13)

project ("CuriousX")
set(CMAKE_CXX_EXTENSIONS OFF)


# -------------------- Fetch googletest content--------------------

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.1
)

FetchContent_GetProperties(googletest)

if (NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()


# Todo add option to build test
enable_testing()



# todo: remove ?
add_executable (CuriousX "main.cpp")

# -------------------- Enforce cpp 17 --------------------

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
else()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif()


# -------------------- add subdirectories --------------------
include_directories(Utils)
include_directories(tests)
add_subdirectory(LexicalAnalysis)

target_link_libraries( ${PROJECT_NAME} LexicalAnalysis)



