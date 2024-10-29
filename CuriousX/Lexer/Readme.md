# Lexical Analysis Component

The Lexical Analyzer (Lexer) is the first phase of the CuriousX compiler. It breaks down source code into a sequence of meaningful tokens, handling the lowest level of syntactic analysis.

## Overview

The Lexer scans the input character by character, identifying tokens such as identifiers, numbers, operators, and keywords. Each token contains:
- The actual text (value)
- The type of token
- The location in the source code

## Token Structure

```cpp
struct LexerToken {
    std::string value;        // The actual text of the token
    LexerTokenType type;      // The category/type of the token
    SourceLocation source;    // Location in source code (line, column)
};
```

### Token Types

The Lexer recognizes the following token types:

```cpp
enum class LexerTokenType {
    // Delimiters
    ParenOpen,      // (
    ParenClose,     // )
    
    // Literals
    FloatToken,     // 3.14
    IntToken,       // 42
    StringToken,    // "hello"
    
    // Identifiers
    VarToken,       // variable names
    
    // Operators
    PlusToken,      // +
    MinusToken,     // -
    DivideToken,    // /
    MultiplyToken,  // *
    AssignToken,    // =
    
    // Keywords
    PrintToken,     // print
    
    // Whitespace
    Space,          // ' '
    Tab,           // '\t'
    Newline,       // '\n'
    
    // Special
    Eof,           // End of file
    Unknown        // Invalid token
};
```

## Features

- **Location Tracking**: Maintains precise line and column information
- **Error Handling**: Reports lexical errors with location information
- **Whitespace Handling**: Properly manages spaces, tabs, and newlines
- **Comment Support**: Handles single-line comments
- **Number Recognition**: Supports both integer and floating-point numbers

## Example

### Input Source
```cpp
x = 5
y = 2 + 3 * 5 - (8 / 3)
z = x + y
print(z)
```

### Generated Tokens
```plaintext
==== Lexer Output ====
Token               Position            Value
-----------------------------------------------
VarToken            <line:1, col:1>     [x]
AssignToken         <line:1, col:3>     [=]
IntToken            <line:1, col:5>     [5]
VarToken            <line:2, col:1>     [y]
AssignToken         <line:2, col:3>     [=]
IntToken            <line:2, col:5>     [2]
PlusToken           <line:2, col:7>     [+]
IntToken            <line:2, col:9>     [3]
MultiplyToken       <line:2, col:11>    [*]
...
```

## Implementation Details

### Token Generation Process
1. **Character Reading**: Reads input source character by character
2. **Token Recognition**: Uses pattern matching to identify tokens
3. **Location Tracking**: Updates line and column numbers

## Error Handling

The Lexer handles various error conditions:
- Invalid characters
- Malformed numbers
- Unterminated strings
- Unknown symbols

Example error message:
```plaintext
Error: Invalid character '@' at line 1, column 5
```

## Usage in the Compiler Pipeline

The Lexer is used by the Parser to obtain a stream of tokens:
```cpp
Lexer lexer(sourceCode);
Token token;
while ((token = lexer.nextToken()).type != LexerTokenType::Eof) {
    // Process token
}
```

## Learn More
For detailed information about the lexical analysis process, visit:
[CuriousX Lexical Analyzer Blog](https://jenniferchukwu.com/posts/lexicalAnalysis)
