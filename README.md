![CI](https://github.com/jnyfah/CuriousX/actions/workflows/ci.yml/badge.svg)

# CuriousX

A small compiler, written from scratch in C++23, for a minimal statically-typed
language with inferred types. Built to learn how compilers work end to end —
from characters on disk to machine code.

[Read more](https://jenniferchukwu.com/posts/curiousx)

> **Status:** the front end is complete — lexer, parser, and semantic analysis
> all working and tested. The back end is being written; see [Roadmap](#roadmap).

## The language

```
func add(a, b) {
    return a + b;
}

x = 10;
y = add(x, 5);

if (y > 12) {
    print("big");
} else {
    print(y);
}

while (x > 0) {
    x = x - 1;
}
```

- **Types** — `integer`, `float`, `boolean`, `string`. Never written down:
  every type is inferred, including function parameters and return types.
- **Operators** — `+ - * / %`, `== != < <= > >=`, `& |`, unary `-` and `!`
- **Control flow** — `if` / `else if` / `else`, `while`
- **Functions** — top-level only, recursion and mutual recursion supported,
  arguments passed by value
- **Built-ins** — `print`

Comments run from `#` to the end of the line. Statements end in `;`.

### Type inference

There are no type annotations anywhere. Local variables take the type of the
value assigned to them. A function's signature is fixed by its **first call**:

```
func double(n) { return n * 2; }

x = double(3);      # double is now (integer) -> integer
y = double(1.5);    # error: argument 1 of 'double': expected integer, found float
```

Function bodies are therefore analysed on demand, in call-graph order rather
than source order, so a function that is never called is never type-checked
(you get a warning).

## Architecture

```
source ─► Lexer ─► Parser ─► Sema ─► [ IR ] ─► [ RISC-V ]
            │        │         │
          tokens    AST    types + slots
```

| Layer | Directory | Responsibility |
|---|---|---|
| Helpers | `src/helpers/` | source locations, diagnostics, the type enum |
| Lexer | `src/lexer/` | hand-written scanner; whitespace and comments are trivia |
| Parser | `src/parser/` | recursive descent with precedence climbing; arena-allocated AST |
| Sema | `src/sema/` | scoped symbol table, type inference, name resolution, frame slots |

Dependencies point one way only. Nothing below a layer knows about the layers
above it, and the eventual back end will consume only the IR.

Some things worth knowing about how it is built:

- **Errors are values, not exceptions.** Every phase reports into a shared
  `Diagnostics` engine and keeps going, so one run surfaces every error rather
  than the first. The parser recovers with panic-mode synchronisation and emits
  poisoned `Error` nodes so a single syntax error does not cascade.
- **The AST lives in an arena.** Every node dies at the same moment, so nodes
  are bump-allocated and freed wholesale. Nodes hold `std::span` children rather
  than vectors; non-trivially-destructible objects register a finaliser.
- **Sema annotates the tree.** Each expression node is stamped with its type,
  and each identifier with the **slot** it occupies in its function's stack
  frame. Later phases read those off the node instead of re-deriving them —
  by codegen time the scopes no longer exist.
- **Top-level code is a function.** Statements outside any `func` become the
  body of an implicit `@main`, so every variable in a program belongs to a
  frame and has a real slot. There are no global variables.

## Building

Requires **CMake 4.0+** and a C++23 compiler (GCC 13+).

```bash
cmake -S . -B build
cmake --build build -j
./build/CuriousX
```

### Tests

```bash
cd build && ctest --output-on-failure
```

104 tests covering the lexer, parser (including error recovery and
associativity), and semantic analysis.

### Sanitizers

```bash
cmake -S . -B build-asan -DENABLE_SANITIZERS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build-asan -j
cd build-asan && ctest --output-on-failure
```

Builds with ASan and UBSan. Warnings are errors by default
(`-DWARNINGS_AS_ERRORS=OFF` to relax).

## Roadmap

- [x] Lexer
- [x] Parser with error recovery
- [x] Semantic analysis: inference, scoping, frame slots
- [ ] Tree-walking interpreter — a reference implementation to diff the back end against
- [ ] IR: three-address, virtual registers, basic blocks
- [ ] RISC-V code generation, running under Spike
- [ ] Bare metal on an FPGA soft-core

## Licence

Boost Software License 1.0. See [LICENSE](LICENSE).
