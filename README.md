[![Travis](https://img.shields.io/travis/jnyfah/CuriousX/master.svg?style=flat&logo=travis&color=yellow)](https://app.travis-ci.com/jnyfah/CuriousX)
![Github](https://github.com/jnyfah/CuriousX/actions/workflows/cmake.yml/badge.svg)
![Github](https://github.com/jnyfah/CuriousX/actions/workflows/unittest.yml/badge.svg)
![Github](https://github.com/jnyfah/CuriousX/actions/workflows/msvc.yml/badge.svg)
![Github](https://github.com/jnyfah/CuriousX/actions/workflows/codeql.yml/badge.svg)
![GitHub](https://img.shields.io/github/license/jnyfah/CuriousX?color=blue&logo=github)
![GitHub top language](https://img.shields.io/github/languages/top/jnyfah/CuriousX?color=red)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/400c60bf7e00462d880d5d782adec10e)](https://www.codacy.com/gh/jnyfah/CuriousX/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=jnyfah/CuriousX&amp;utm_campaign=Badge_Grade)

# CuriousX: A Miniature Compiler
CuriousX is a mini subset of C++ language created for fun and educational purposes, to help understand how compilers work and how to build a simple programming language.  

compilers are just one part of the toolchain that help build programs from source code to an executable, to learn more visit [what is CuriousX](https://jenniferchukwu.com/posts/curiousx)

CuriousX is designed to mimic the structure of a typical compiler and includes features such as lexical analysis, syntax analysis, and semantic analysis. Additionally, it includes a visualization of the output from each stage of the compiler.

## Features
-   Print
-   Variables (integer and float)
-   Expressions (add, subtract, divide, and multiply)
-   Operator precedence
-   Statements
-   __Target Architecture:__ X86 and ARM

## Dependencies
1.  A C++ compiler that supports C++17. See [cppreference.com](https://en.cppreference.com/w/cpp/compiler_support) to see which features are supported by each compiler.
2.  [CMake 3.15+](https://cmake.org/)

## Building

```cmd
mkdir build
cd build
cmake ..
cmake --build .
```

## Testing
To compile and run the tests, you need to turn on the `-DCURIOUSX_BUILD_TESTS` option when building and execute

```cmd
cd build
cmake -DCURIOUSX_BUILD_TESTS=ON ..
cmake --build .
ctest
```
