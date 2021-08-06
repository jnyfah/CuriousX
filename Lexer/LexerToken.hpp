#ifndef LEXERTOKEN_HPP
#define LEXERTOKEN_HPP

#include <string>
#include <string_view>

#include "../utils/sourcelocation.hpp"

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
    EqualToken,
    PrintToken,

    ArrowToken,
    SemicolonToken,
    Space,
    Tab,
    Newline,
    Eof,

    Unkown,
};

//! Converts LexerToken to String
const char* toString(LexerTokenType t);

//! Represents a single token in the expression stream
struct LexerToken {
    std::string_view value;
    SourceLocation location;
    LexerTokenType type;

    std::string toString() const {
        return std::string("[") + std::string(value) + "] -> \t" + location.toString() + std::string(";\t ") + ::toString(type);
    }
};

#endif // LEXERTOKEN_HPP
