# Syntax Analysis Component

The Syntax Analyzer (Parser) is the second phase of the CuriousX compiler. It processes the token stream from the Lexer and builds an Abstract Syntax Tree (AST) according to the language grammar.

## Overview

The Parser implements a recursive descent parsing strategy with the following features:
- Operator precedence handling
- Error recovery
- Abstract Syntax Tree (AST) generation
- Support for expressions, statements, and control structures

## Grammar Specification

### Formal Grammar
```ebnf
<program> ::= <statement> { <statement> }

<statement> ::= <expr>
              | <print_stmt>
              | <if_stmt>

<print_stmt> ::= 'print' <expr>

<if_stmt> ::= 'if' '(' <expr> ')' <block> [ 'else' <block> ]

<block> ::= '{' <statement> { <statement> } '}'
          | <statement>

<expr> ::= <term> { <addop> <term> }

<term> ::= <factor> { <mulop> <factor> }

<factor> ::= '(' <expr> ')'
           | <number>
           | <identifier>
           | <bool_literal>

<addop> ::= '+' | '-'
<mulop> ::= '*' | '/'
<bool_literal> ::= 'true' | 'false'
```

### Operator Precedence
1. Parentheses `()`
2. Multiplication and Division `* /`
3. Addition and Subtraction `+ -`
4. Assignment `=`

## AST Node Structure

The Parser generates an Abstract Syntax Tree using these node types:

```cpp
enum class NodeType {
    BinaryOperation,      // Operations with two operands
    ConditionalOperation, // If-else statements
    BlockOperation,       // Code blocks
};

class ASTNode {
public:
    ASTNode(const LexerToken& token) : token(token) {}
    virtual ~ASTNode() = default;
    virtual NodeType getType() const = 0;
    LexerToken token;
};

// Example of concrete node types
class BinaryNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    NodeType getType() const override { return NodeType::BinaryOperation; }
};
```

## Parsing Examples

### Basic Expression
Input:
```cpp
x = 5 + 2 * 3
```

Generated AST:
```plaintext
          =
        /   \
       x     +
            / \
           5   *
              / \
             2   3
```

### If Statement
Input:
```cpp
if (x > 0) {
    print(x)
}
```

Generated AST:
```plaintext
        if
       /  \
      >   block
     / \    |
    x   0  print
            |
            x
```


## Common Grammar Rules

1. **Expressions**
   - Binary operations (arithmetic, comparison)
   - Variable assignments
   - Parenthesized expressions

2. **Statements**
   - Expression statements
   - Print statements
   - If-else statements
   - Blocks

3. **Control Flow**
   - Conditional branching
   - Block scoping

## Error Handling

The Parser handles various syntax errors:
- Unexpected tokens
- Missing delimiters
- Invalid expressions
- Malformed statements

Error example:
```plaintext
Error at line 3: Expected ')' after expression
if (x > 0 {
         ^
```