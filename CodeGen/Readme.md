## Code Generation
The code generator is responsible for generating assembly code from an abstract syntax tree (AST) produced by the language's parser. 
The register allocator manages the allocation and de-allocation of registers during code generation.

The code generator is implemented in `codegen.cpp`. It takes an AST as input and outputs ARMv8 assembly code. It code generator traverses the AST in postfix order and generates code for each node in the AST. 
 
The output is written to a std::stringstream object and saved to a file

The code generator supports the following AST nodes:

-   AssignNode
-   IntNode
-   FloatNode
-   VarNode
-   PlusNode
-   MinusNode
-   MultiplyNode
-   DivideNode
-   PrintNode

### Register Allocator
The register allocator is implemented in `register.hpp`. It manages the allocation and de-allocation of registers during code generation. The register allocator supports a maximum of 1- registers, and it uses a stack-based allocation strategy.

When a register is requested, the register allocator searches for an available register. If no registers are available, it spills the register with the lowest priority to memory. When a register is freed, the register allocator marks it as available for reuse.

Here's an example of using the code generator and register allocator to generate assembly code for the following source code:

```c
x = 5 + 2 * 3
```

The AST for this source code is:
```sh
                         =
                        / \
                       x   +
                          / \
                         5   *
                            / \
                           2   3

```
The code generator generates the following assembly code:
```sh
	 mov r0, #5
	 mov r1, #2
	 mov r2, #3
	 mul r1, r1, r2
	 add r0, r0, r1
	 str r0, [sp, #-4]!

	 bx lr
```