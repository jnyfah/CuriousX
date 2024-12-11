# WebAssembly Code Generation Component

The WebAssembly Code Generator (WasmGen) is the final phase of the CuriousX compiler. It transforms the typed AST into WebAssembly instructions that can be executed in web browsers or other WASM environments.

## Core Components

### 1. Instruction Set
```cpp
enum class WasmInstruction {
    // Integer Operations
    I32Const,    // Push integer constant
    I32Add,      // Add integers
    I32Sub,      // Subtract integers
    I32Mul,      // Multiply integers
    I32DivS,     // Signed integer division
    
    // Float Operations
    F32Const,    // Push float constant
    F32Add,      // Add floats
    F32Sub,      // Subtract floats
    F32Mul,      // Multiply floats
    F32Div,      // Divide floats
    
    // Comparisons
    I32Eq,       // Integer equality
    I32Ne,       // Integer inequality
    I32LtS,      // Integer less than
    I32GtS,      // Integer greater than
    I32LeS,      // Integer less or equal
    I32GeS,      // Integer greater or equal
    
    // Local Variables
    LocalGet,    // Get local variable
    LocalSet,    // Set local variable
    
    // Control Flow
    If,          // If statement
    Else,        // Else branch
    End,         // End block
    CallPrint    // Call print function
};
```

### 2. Instruction Representation
```cpp
struct WasmInstructionWithData {
    WasmInstruction instruction;
    std::string data;
    bool has_data;
    
    explicit WasmInstructionWithData(WasmInstruction instr);
    WasmInstructionWithData(WasmInstruction instr, std::string value);
};
```

## Code Generation Examples

### 1. Basic Arithmetic
```cpp
// Source: x = 5 + 3
// AST:
//       =
//      / \
//     x   +
//        / \
//       5   3

// Generated WASM:
i32.const 5    // Push 5
i32.const 3    // Push 3
i32.add        // Add top two values
local.set 0    // Store in variable x
```

### 2. Complex Expression
```cpp
// Source: y = (2 * 3) + (4 / 2)
// Generated WASM:
i32.const 2    // Push 2
i32.const 3    // Push 3
i32.mul        // Multiply
i32.const 4    // Push 4
i32.const 2    // Push 2
i32.div_s      // Divide
i32.add        // Add results
local.set 1    // Store in variable y
```

### 3. Conditional Statement
```cpp
// Source:
if (x > 0) {
    print(x)
}

// Generated WASM:
local.get 0    // Get x
i32.const 0    // Push 0
i32.gt_s       // Compare x > 0
if            // Start if block
    local.get 0  // Get x for print
    call $print  // Call print
end           // End if block
```
