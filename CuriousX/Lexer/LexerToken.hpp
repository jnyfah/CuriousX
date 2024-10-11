#pragma once

#include "SourceLocation.hpp"
#include "Error.hpp"
#include <string_view>

enum class LexerTokenType
{
    ParenOpen,
    ParenClose,
    BracesOpen,
    BracesClose,

    FloatToken,
    IntToken,
    VarToken,
    StringToken,

    IfToken,
    ElseToken,
    BoolToken,

    PlusToken,
    MinusToken,
    DivideToken,
    MultiplyToken,
    AssignToken,
    PrintToken,

    EqualToken,
    NotEqualToken,
    LessToken,
    LessEqualToken,
    GreaterToken,
    GreaterEqualToken,

    CommentToken,
    Space,
    Tab,
    Newline,
    Eof,

    ProgramToken,
    Unknown
};

//! Represents a single token in the expression stream
struct LexerToken
{
    std::string_view value;
    SourceLocation location;
    LexerTokenType type;
};

//! Converts LexerToken to String
const char* toString(const LexerTokenType& t);