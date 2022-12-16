#include "LexerToken.hpp"

const char* toString(LexerTokenType t)
{
    switch (t)
    {
        case LexerTokenType::ParenOpen:
            return "ParenOpen";
        case LexerTokenType::ParenClose:
            return "ParenClose";
        case LexerTokenType::FloatToken:
            return "FloatToken";
        case LexerTokenType::IntToken:
            return "IntToken";
        case LexerTokenType::VarToken:
            return "VarToken";

        case LexerTokenType::PlusToken:
            return "PlusToken";
        case LexerTokenType::MinusToken:
            return "MinusToken";
        case LexerTokenType::DivideToken:
            return "DivideToken";
        case LexerTokenType::MultiplyToken:
            return "MultiplyToken";
        case LexerTokenType::AssignToken:
            return "AssignToken";
        case LexerTokenType::PrintToken:
            return "PrintToken";

        case LexerTokenType::Space:
            return "Space";
        case LexerTokenType::Tab:
            return "Tab";
        case LexerTokenType::Newline:
            return "Newline";
        case LexerTokenType::Eof:
            return "Eof";

        case LexerTokenType::Unknown:
            return "Unknown";
        default:
            break;
    }
    return "<<BAD TYPE>>";
}