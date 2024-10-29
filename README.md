![Github](https://github.com/jnyfah/CuriousX/actions/workflows/cmake.yml/badge.svg)


# CuriousX: A Miniature Compiler
CuriousX is a modern compiler that translates a C++-like language into WebAssembly. Built for educational purposes, it demonstrates fundamental compiler concepts through a practical implementation.

[Try the Online Compiler](https://jnyfah.github.io/CuriousX/) | [Learn More](https://jenniferchukwu.com/posts/curiousx)

## Features

### Language Support
- **Data Types**
  - Integers and Floating-point numbers
  - Strings (basic support)
  - Booleans
- **Operations**
  - Arithmetic: `+`, `-`, `*`, `/`
  - Comparison: `==`, `!=`, `<=`, `>=`, `>`,`<`
- **Control Flow**
  - If-else statements
  - Basic blocks
- **Functions**
  - Built-in `print()` function
  - Variable declarations and assignments

### Compiler Pipeline
1. **Lexical Analysis**
   - Token generation
   - Source location tracking
   - Comment handling
2. **Syntax Analysis**
   - AST generation
   - Operator precedence
   - Error recovery
3. **Semantic Analysis**
   - Type checking
   - Scope analysis
   - Symbol table management
4. **Code Generation**
   - WebAssembly text format output
   - Optimization passes
   - Runtime support

## Getting Started

### Prerequisites
- C++17 compatible compiler
- CMake 3.25 or higher
- [Emscripten](https://emscripten.org/docs/getting_started/downloads.html) (for WebAssembly compilation)
- Git for version control

### Building the Project

#### Standard Build (Native)
```bash
# Clone the repository
git clone https://github.com/jnyfah/CuriousX.git
cd CuriousX

# Create build directory and build
mkdir build
cmake -B build -S .
cmake --build build
```

#### WebAssembly Build
```bash
# Configure with Emscripten
emcmake cmake -B build -S .

# Build
cmake --build build

# Start local server
emrun CompilerEditor/index.html
```

### Usage Examples

#### Basic Arithmetic
```cpp
x = 42
y = x + 8
if (y > 0) {
    print("Positive")
} else {
    print("Non-positive")
}
```

#### Output Examples

##### Lexical Analysis
```plaintext
Newline             line:1, col:26           [\n]
VarToken            line:2, col:1            [x]
AssignToken         line:2, col:3            [=]
IntToken            line:2, col:5            [42]
```

##### Abstract Syntax Tree
```plaintext
     =
    / \
   x   42
```

##### Generated WebAssembly
```wasm
i32.const 42
local.set 0    ;; x = 42
local.get 0
call $print    ;; print(x)
```
## Project Structure
```
CuriousX/
├── CompilerUtils/    # Utility classes and helpers
├── CuriousX/        # Core compiler implementation
│   ├── Lexer/       # Lexical analysis
│   ├── Parser/      # Syntax analysis
│   ├── Semantic/    # Semantic analysis
│   └── Generation/  # Code generation
├── tests/          # Unit and integration tests
└── CompilerEditor/ # Web interface
```

## Testing
The project includes comprehensive tests for each compiler component:
```bash
# Build with tests enabled
cmake -B build -DBUILD_TESTS=ON
cmake --build build

# Run tests
cd build && ctest -C Debug -V
```

## Further Reading
- [Lexer Design Documentation](CuriousX\Lexer\Readme.md)
- [WebAssembly Output Format](CuriousX\Generation\README.md)