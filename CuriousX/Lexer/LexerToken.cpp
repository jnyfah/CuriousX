#include "LexerToken.hpp"

const char *toString(const LexerTokenType& t)
{
    switch (t)
    {
    case LexerTokenType::ParenOpen:
        return "ParenOpen";
    case LexerTokenType::ParenClose:
        return "ParenClose";
    case LexerTokenType::BracesOpen:
        return "BracesOpen";
    case LexerTokenType::BracesClose:
        return "BracesClose";


    case LexerTokenType::FloatToken:
        return "FloatToken";
    case LexerTokenType::IntToken:
        return "IntToken";
    case LexerTokenType::VarToken:
        return "VarToken";
    case LexerTokenType::StringToken:
        return "StringToken";
    
    case LexerTokenType::IfToken:
        return "IfToken";
    case LexerTokenType::ElseToken:
        return "ElseToken";
    case LexerTokenType::BoolToken:
        return "BoolToken";

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

    case LexerTokenType::EqualToken:
        return "EqualToken";
    case LexerTokenType::NotEqualToken:
        return "NotEqualToken";
    case LexerTokenType::LessEqualToken:
        return "LessEqualToken";
    case LexerTokenType::LessToken:
        return "LessToken";
    case LexerTokenType::GreaterEqualToken:
        return "GreaterEqualToken";
    case LexerTokenType::GreaterToken:
        return "GreaterToken";


    case LexerTokenType::CommentToken:
        return "Comment";      
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