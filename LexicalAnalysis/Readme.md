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
Some example input files have been provided in (`Utils/input.txt`) so as to see what tokens the lexer finds in the file

```c++
x = 5
y = 2 + 3 * 5 - (8 / 3)
z = x + y
print(z)
```

Output (`Lexical-analysis.txt`) 

```sh
[x]    ->   <line:1, col:1>;	 VarToken
[=]    ->   <line:1, col:3>;	 AssignToken
[5]    ->   <line:1, col:5>;	 IntToken
[y]    ->   <line:2, col:1>;	 VarToken
[=]    ->   <line:2, col:3>;	 AssignToken
[2]    ->   <line:2, col:5>;	 IntToken
[+]    ->   <line:2, col:7>;	 PlusToken
[3]    ->   <line:2, col:9>;	 IntToken
[*]    ->   <line:2, col:11>;	 MultiplyToken
[5]    ->   <line:2, col:13>;	 IntToken
[-]    ->   <line:2, col:15>;	 MinusToken
[(]    ->   <line:2, col:17>;	 ParenOpen
[8]    ->   <line:2, col:18>;	 IntToken
[/]    ->   <line:2, col:20>;	 DivideToken
[3]    ->   <line:2, col:22>;	 IntToken
[)]    ->   <line:2, col:23>;	 ParenClose
[z]    ->   <line:3, col:1>;	 VarToken
[=]    ->   <line:3, col:3>;	 AssignToken
[x]    ->   <line:3, col:5>;	 VarToken
[+]    ->   <line:3, col:7>;	 PlusToken
[y]    ->   <line:3, col:9>;	 VarToken
[print] ->   <line:4, col:1>;	 PrintToken
[(]    ->   <line:4, col:6>;	 ParenOpen
[z]    ->   <line:4, col:7>;	 VarToken
[)]    ->   <line:4, col:8>;	 ParenClose
```