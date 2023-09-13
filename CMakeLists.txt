cmake_minimum_required (VERSION 3.13)

project ("CuriousX")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# todo: remove ?
add_executable (CuriousX "main.cpp")

# -------------------- Enforce cpp 17 --------------------

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
else()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif()



# Todo add option to build test

# -------------------- Options --------------------------------
option( CURIOUSX_BUILD_TESTS "Specifies whether to build the tests" OFF )
option( CURIOUSX_BUILD_EXAMPLES "Specifies whether to build the examples" OFF )

# -------------------- add subdirectories --------------------
include_directories(Utils)
add_subdirectory(CodeGen)
add_subdirectory(SemanticAnalysis)
add_subdirectory(SyntaxAnalysis)
add_subdirectory(LexicalAnalysis)


#add_dependencies(SyntaxAnalysis LexicalAnalysis) # subB waits for subA to build first

if(CURIOUSX_BUILD_TESTS) 

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
  enable_testing()
  add_subdirectory(tests)

endif()

# Set the output directory for the executable
set_target_properties(CuriousX PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/web"
)

target_link_libraries( ${PROJECT_NAME} LexicalAnalysis SyntaxAnalysis SemanticAnalysis CodeGen)
set_target_properties(CuriousX PROPERTIES LINK_FLAGS "--bind -s DISABLE_EXCEPTION_CATCHING=0")

