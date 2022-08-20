
## Lexer

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

```c
enum class LexerTokenType {

  ParenOpen,
  ParenClose,
  BraceOpen,
  BraceClose,
  FloatToken,
  IntToken,
  VarToken,

  PlusToken,
  MinusToken,
  DivideToken,
  MultiplyToken,
  AssignToken,
  PrintToken,

  IfToken,
  ElseToken,
  NotToken,

  GreaterThanToken,
  LessThanToken,
  GreaterEqualToken,
  LessEqualToken,
  NotEqualToken,
  EqualToken,

  SemicolonToken,
  Space,
  Tab,
  Newline,
  Eof,

  Unknown
};
```


## Some Example Input Files

Some example input files have been provided in (`Utils/input.txt`) so as to see what tokens the lexer finds in the file

```
x = 5
#
2 + 3 * 5 - (8 / 3)
x->2.54
print(x)
```

Output (`Lexer.cpp`) 

```
[x]       VarToken       <line:1, col:0>
[ ]       Space          <line:1, col:1>
[=]       EqualToken     <line:1, col:2>
[ ]       Space          <line:1, col:3>
[5]       IntToken       <line:1, col:4>
[#]       unknown        <line:2, col:0>
[2]       IntToken       <line:3, col:0>
[ ]       Space          <line:3, col:1>
[+]       PlusToken      <line:3, col:2>
[ ]       Space          <line:3, col:3>
[3]       IntToken       <line:3, col:4>
[ ]       Space          <line:3, col:5>
[*]       MultiplyToken  <line:3, col:6>
[ ]       Space          <line:3, col:7>
[5]       IntToken       <line:3, col:8>
[ ]       Space          <line:3, col:9>
[-]       MinusToken     <line:3, col:11>
[(]       ParenOpen      <line:3, col:12>
[8]       IntToken       <line:3, col:13>
[ ]       Space          <line:3, col:14>
[/]       DivideToken    <line:3, col:15>
[ ]       Space          <line:3, col:16>
[3]       IntToken       <line:3, col:17>
[)]       ParenClose     <line:3, col:18>
[x]       VarToken       <line:4, col:0>
[-]       MinusToken     <line:4, col:2>
[2.54]    FloatToken     <line:4, col:3>
[print]   PrintToken     <line:5, col:0>
[(]       ParenOpen      <line:5, col:5>
[x]       VarToken       <line:5, col:6>
[)]       ParenClose     <line:5, col:7>

```

