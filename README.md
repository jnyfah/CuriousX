[![Travis](https://img.shields.io/travis/jnyfah/CuriousX/master.svg?style=flat&logo=travis&color=yellow)](https://app.travis-ci.com/jnyfah/CuriousX)
![Github](https://github.com/jnyfah/CuriousX/actions/workflows/cmake.yml/badge.svg)
![Github](https://github.com/jnyfah/CuriousX/actions/workflows/unittest.yml/badge.svg)
![Github](https://github.com/jnyfah/CuriousX/actions/workflows/msvc.yml/badge.svg)
![Github](https://github.com/jnyfah/CuriousX/actions/workflows/codeql.yml/badge.svg)
![GitHub](https://img.shields.io/github/license/jnyfah/CuriousX?color=blue&logo=github)
![GitHub top language](https://img.shields.io/github/languages/top/jnyfah/CuriousX?color=red)

# CuriousX

This repo is for people who are curious about how compilers work, compilers are just one part of the toolchain that help  build programs from source code to executable to learn more visit [what is CuriousX](https://jenniferchukwu.com/posts/buildsystems2)


CuriousX is a mini subset of C++ language created just for fun and out of curiosity, a visualization of each stage would also be created and maybe a web playground too !!

## Features
- Print
- Variables (strings, integer and float)
- if/else statements
- Expressions (add, subtract, divide, and multiply)
- Operator precedence
- Operator comparisons
- Statements
- __Target Architecture:__ X86 and ARM

## Dependencies
1. A C++ compiler that supports C++17. See [cppreference.com](https://en.cppreference.com/w/cpp/compiler_support) to see which features are supported by each compiler.

2. [CMake 3.15+](https://cmake.org/)

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