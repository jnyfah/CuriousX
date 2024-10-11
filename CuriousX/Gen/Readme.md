## WebAssembly Code Generation

The WasmGen class is responsible for generating WebAssembly (WASM) code from an Abstract Syntax Tree (AST) produced by the parser. The WasmGen traverses the AST nodes and generates WebAssembly instructions in the correct order for the operations being performed.

The code generation logic is implemented in `WasmGen.cpp`. It accepts an AST as input, walks through the nodes, and outputs a series of WebAssembly instructions. These instructions are stored in an internal list and can be accessed via a `std::vector<WasmInstructionWithData>`. The generator handles variable assignments, expressions, arithmetic operations, conditionals, and control flow for WebAssembly code.

### WebAssembly Instructions

The code generator supports the following WebAssembly operations:

- Integer Arithmetic: `i32.add`, `i32.sub`, `i32.mul`, `i32.div_s`
- Floating-point Arithmetic: `f32.add`, `f32.sub`, `f32.mul`, `f32.div`
- Comparison Operators: Both integer (`i32.eq`, `i32.ne`, etc.) and floating-point (`f32.eq`, `f32.lt`, etc.) comparisons.
- Local Variables: local.get and local.set for reading and writing variables.
- Control Flow: `if`, `else`, `end` for conditionals, and call for function invocation (e.g., calling the print function).

### Variable Handling

WasmGen maintains a mapping of local variable names to their indices in the WebAssembly stack. When variables are assigned, it ensures they are properly tracked and referenced in subsequent operations using local.get and local.set.

Here is an example of generating WebAssembly instructions for the following source code:

```cpp
x = 5 + 2 * 3;
```
```cpp
                         =
                        / \
                       x   +
                          / \
                         5   *
                            / \
                           2   3

```
```cpp
i32.const 5
i32.const 2
i32.const 3
i32.mul
i32.add
local.set 0
```