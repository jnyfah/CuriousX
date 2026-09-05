#pragma once

#include <string>
#include <string_view>
#include "helpers/location.hpp"

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

        Semicolon,
        Comma,
        Eof,

        Unknown
    };

    //! Represents a single token in the expression stream
    struct Token
    {
        std::string_view value;
        Location         location;
        TokenType        type;
    };

    //! Given the tokentype return the char equivalent
    const char* describe(TokenType t);

    //! Given the Token return the char equivalent of the tokentype
    std::string describe(const Token& t);

} // namespace cx