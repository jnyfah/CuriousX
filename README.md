![Github](https://github.com/jnyfah/CuriousX/actions/workflows/cmake.yml/badge.svg)
![Github](https://github.com/jnyfah/CuriousX/actions/workflows/msvc.yml/badge.svg)
![Github](https://github.com/jnyfah/CuriousX/actions/workflows/codeql.yml/badge.svg)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/400c60bf7e00462d880d5d782adec10e)](https://www.codacy.com/gh/jnyfah/CuriousX/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=jnyfah/CuriousX&amp;utm_campaign=Badge_Grade)

# CuriousX: A Miniature Compiler
CuriousX is a mini subset of C++ language created for fun and educational purposes, to help understand how compilers work and how to build a simple programming language.  

compilers are just one part of the toolchain that help build programs from source code to an executable, to learn more visit [what is CuriousX](https://jenniferchukwu.com/posts/curiousx)

CuriousX is designed to mimic the structure of a typical compiler and includes features such as lexical analysis, syntax analysis, and semantic analysis. Additionally, it includes a visualization of the output from each stage of the compiler.

__Test and check out the [Compiler Playground](https://jnyfah.github.io/CuriousX/)__

## Features
-   Print
-   Variables (integer and float)
-   Expressions (add, subtract, divide, and multiply)
-   Operator precedence
-   Statements
-   __Target Architecture:__ ARMv8

## Dependencies
1.  A C++ compiler that supports C++17. See [cppreference.com](https://en.cppreference.com/w/cpp/compiler_support) to see which features are supported by each compiler.
2.  [CMake 3.25+](https://cmake.org/)
3.  Compiler toolchain to WebAssembly [Emscripten](https://emscripten.org/docs/getting_started/downloads.html)

## Building

### Building Without Emscripten (Standard Build)
```cmd
mkdir build
cd build
cmake -B build -S .
cmake --build build
```
This will compile the project using your default system compiler (e.g., GCC, Clang, or MSVC).

### Building with Emscripten (WebAssembly Deployment)
```cmd
emcmake cmake -B build -S .
cmake --build build
```
This will configure the project to be compiled using Emscripten, allowing you to deploy it as WebAssembly

__Note:__ Emscripten might not compatible with the `Visual Studio generator` due to a lack of `MSBuild` integration. Use `Ninja` or `Makefiles` instead.

## Usage
html file is located at the `web` folder, after building sucessfully, start the webserver
```cmd
emrun CompilerEditor/index.html
```

This generates a `Lexical-analysis` `Syntax-analysis` and an `assembly` files outputs on the webpage and you can also view the symbol table if you source code has any.

You can always validate your assembly code with this ARM emulator [miniarm](https://github.com/ebresafegaga/miniarm)

or Just visit the __[Compiler Playground](https://jnyfah.github.io/CuriousX/)__ 🫠

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

TODO:
- [ ] the sun for the toggle theme
- [ ] code line numbers
- [ ] color code area
- [ ] being able to scroll when input/output overflows
- [ ] Clearing previous output when exception occurs ( clear output anytime compile is hit ?)
- [ ] if output file is not provided create one
- [ ] what if comment is too long
- [ ] remove dark mode check prettier for good design
- [ ] writing Unit tests
- [ ] Translation validation