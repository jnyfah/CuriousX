#ifndef LEXER_HPP
#define LEXER_HPP
#include "sourcelocation.hpp"
#include <string>
#include <vector>


enum class LexerTokenType {

    ParenOpen,
    ParenClose,
    FloatToken,
    IntToken,
    VarToken,

    PlusToken,
    MinusToken,
    MultiplyToken,
    DivideToken,
    EqualToken,

    ArrowToken,
    SemiColonToken,
    Space,
    Tab,
    NewLine,
    Eof,

    unknown

};

struct Token {
        std::string value;
        LexerTokenType type;
        SourceLocation source;
      };

class Lexer {
    public:
       Token tokenizeCharacter(std::string, unsigned short, unsigned short);
       const char* toString(LexerTokenType );
       std::vector<Token>Tokenize(std::string, unsigned short);
       bool IsWhiteSpace(char);
       bool IsEndOfLine(char);
       bool IsNumeric(char);
       bool IsLetter(char);
       
};



#endif 
