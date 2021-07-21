
## Lexer

The job of the Lexer is to identify the lexical elements, or tokens, in the input language.
Each token that we scan is going to be stored in this structure
(from `tokenizer.hpp`):

```c
// Token structure
struct Token {
        std::string value;
        LexerTokenType type;
        SourceLocation source;
};
```
where the `sourcelocation` is the location i.e, row and coloumn of a token, the `type` field can be one of these values (from `tokenizer.hpp`):

```c
enum class LexerTokenType {

    ParenOpen,
    ParenClose,
    FloatToken,
    IntToken,
    VarToken,
    print,

    PlusToken,
    MinusToken,
    MultiplyToken,
    DivideToken,
    EqualToken,

    ArrowToken,
    ColonToken,
    Space,
    Tab,
    NewLine,
    Eof,

    unknown

};
```


## Some Example Input Files

Some example input files have been provided in (`input.txt`) so as to see what tokens
the lexer finds in the file

```
x = 5
2 + 3 * 5 - (8 / 3)
x->2.54
print(x)
```

Output (`lexer.cpp`) 

```
[x]       VarToken       <line:1, col:0>
[ ]       Space          <line:1, col:1>
[=]       <<BAD TYPE>>   <line:1, col:2>
[ ]       Space          <line:1, col:3>
[5]       IntToken       <line:1, col:4>
[2]       IntToken       <line:2, col:0>
[ ]       Space          <line:2, col:1>
[+]       PlusToken      <line:2, col:2>
[ ]       Space          <line:2, col:3>
[3]       IntToken       <line:2, col:4>
[ ]       Space          <line:2, col:5>
[*]       MultiplyToken  <line:2, col:6>
[ ]       Space          <line:2, col:7>
[5]       IntToken       <line:2, col:8>
[ ]       Space          <line:2, col:9>
[-]       MinusToken     <line:2, col:11>
[(]       ParenOpen      <line:2, col:12>
[8]       IntToken       <line:2, col:13>
[ ]       Space          <line:2, col:14>
[/]       DivideToken    <line:2, col:15>
[ ]       Space          <line:2, col:16>
[3]       IntToken       <line:2, col:17>
[)]       ParenClose     <line:2, col:18>
[x]       VarToken       <line:3, col:0>
[-]       MinusToken     <line:3, col:2>
[2.54]    FloatToken     <line:3, col:3>
[print]   PrintToken     <line:4, col:0>
[(]       ParenOpen      <line:4, col:5>
[x]       VarToken       <line:4, col:6>
[)]       ParenClose     <line:4, col:7>
```


