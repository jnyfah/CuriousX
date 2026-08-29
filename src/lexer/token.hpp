#pragma once

#include <string_view>
#include "tools/location.hpp"


namespace cx
{
    enum class TokenType
    {
        ParenOpen,
        ParenClose,
        BracesOpen,
        BracesClose,

        Float,
        Int,
        Var,
        String,

        If,
        Else,
        Bool,
        For,
        While,

        Plus,
        Minus,
        Divide,
        Multiply,
        Percent,

        PlusEq,
        MinusEq,
        DivideEq,
        PercentEq,
        MultiplyEq,

        Assign,
        Print,

        Equal,
        NotEqual,
        Less,
        LessEqual,
        Greater,
        GreaterEqual,

        Func,
        Return,
        Not,
        And,
        Or,

        Comment,
        Semicolon,
        Comma,
        Space,
        Tab,
        Newline,
        Eof,

        Program,
        Unknown
    };

    //! Represents a single token in the expression stream
    struct Token
    {
        std::string_view value;
        Location         location;
        TokenType        type;
    };

    //! Converts LexerToken to String
    const char* toString(const TokenType& t);

} // namespace cx