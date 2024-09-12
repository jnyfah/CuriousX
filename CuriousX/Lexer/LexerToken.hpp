#pragma once

#include <string_view>
#include "SourceLocation.hpp"

enum class LexerTokenType {
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


    Unknown
};

//! Converts LexerToken to String
const char *toString(const LexerTokenType &t);

//! Represents a single token in the expression stream
struct LexerToken
{
    std::string_view value;
    SourceLocation location;
    LexerTokenType type;
};