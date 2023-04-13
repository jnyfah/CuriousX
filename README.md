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
-   __Target Architecture:__ ARMv8

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

## Usage
Executable is located at the `build` folder

```cmd
./CuriousX [txt source code file path]
```
This generates a `Lexical-analysis.txt` `Syntax-analysis.txt` and an `assembly.txt` files located at the build folder and you can also vie the symbol table if you source code has any.

You can always validate your assembly code with this ARM emulator [miniarm](https://github.com/ebresafegaga/miniarm)

## Examples
for the source code input:

```py
a = 2 + 3 * 4
print(a)
```

the lexical-analysis output
```sh
[a]    ->   <line:1, col:1>;	 VarToken
[=]    ->   <line:1, col:3>;	 AssignToken
[2]    ->   <line:1, col:5>;	 IntToken
[+]    ->   <line:1, col:7>;	 PlusToken
[3]    ->   <line:1, col:9>;	 IntToken
[*]    ->   <line:1, col:11>;	 MultiplyToken
[4]    ->   <line:1, col:13>;	 IntToken
[print] ->   <line:2, col:1>;	 PrintToken
[(]    ->   <line:2, col:6>;	 ParenOpen
[a]    ->   <line:2, col:7>;	 VarToken
[)]    ->   <line:2, col:8>;	 ParenClose
```

syntax-analysis output
```sh
          =
        /   \
       a     +
            / \
           2   *
              / \
             3   4
```

and assembly output

```sh
	 mov r0, #2
	 mov r1, #3
	 mov r2, #4
	 mul r1, r1, r2
	 add r0, r0, r1
	 str r0, [sp, #-4]!

print: 
	 ldr r0, [sp, #0]
	 bl printf

	 bx lr
```