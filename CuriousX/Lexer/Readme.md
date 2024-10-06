## Lexical Analysis
The job of the Lexer is to identify the lexical elements, or tokens in the input language.
Each token scanned will be stored in this structure (from `LexerToken.hpp`):

```c
// Token structure
struct Token {
        std::string value;
        LexerTokenType type;
        SourceLocation source;
};
```
where the `SourceLocation` is the location i.e, row and column of a token, the type field can be one of these values (from `LexerToken.hpp`):

```c++
enum class LexerTokenType {

    ParenOpen,
    ParenClose,
    FloatToken,
    IntToken,
    VarToken,

    PlusToken,
    MinusToken,
    DivideToken,
    MultiplyToken,
    AssignToken,
    PrintToken,

    StringToken,

    Space,
    Tab,
    Newline,
    Eof,


    Unknown
};
```

## Some Example Input Files
Some example input files have been provided in (`CompilerUtils/input.txt`)

```c++
x = 5
y = 2 + 3 * 5 - (8 / 3)
z = x + y
print(z)
```

Output (`Lexical-analysis.txt`) 

```sh
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
IntToken            <line:2, col:13>    [5]
MinusToken          <line:2, col:15>    [-]
ParenOpen           <line:2, col:17>    [(]
IntToken            <line:2, col:18>    [8]
DivideToken         <line:2, col:20>    [/]
IntToken            <line:2, col:22>    [3]
ParenClose          <line:2, col:23>    [)]
VarToken            <line:3, col:1>     [z]
AssignToken         <line:3, col:3>     [=]
VarToken            <line:3, col:5>     [x]
PlusToken           <line:3, col:7>     [+]
VarToken            <line:3, col:9>     [y]
PrintToken          <line:4, col:1>     [print]
ParenOpen           <line:4, col:6>     [(]
VarToken            <line:4, col:7>     [z]
ParenClose          <line:4, col:8>     [)]
```
__To learn more:__
## [CuriousX Lexical Analyzer Blog](https://jenniferchukwu.com/posts/lexicalAnalysis)