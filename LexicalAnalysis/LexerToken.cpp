#include "LexerToken.hpp"

const char* toString(LexerTokenType t) {
    switch(t) {
        case LexerTokenType::ParenOpen:         return "ParenOpen";
        case LexerTokenType::ParenClose:        return "ParenClose";
        case LexerTokenType::FloatToken:        return "FloatToken";
        case LexerTokenType::IntToken:          return "IntToken";
        case LexerTokenType::VarToken:          return "VarToken";

        case LexerTokenType::PlusToken:         return "PlusToken";
        case LexerTokenType::MinusToken:        return "MinusToken";
        case LexerTokenType::DivideToken:       return "DivideToken";
        case LexerTokenType::MultiplyToken:     return "MultiplyToken";
        case LexerTokenType::EqualToken:        return "EqualToken";
        case LexerTokenType::PrintToken:        return "PrintToken";

        case LexerTokenType::ArrowToken:        return "ArrowToken";
        case LexerTokenType::SemicolonToken:    return "Semicolon";
        case LexerTokenType::Space:             return "Space";
        case LexerTokenType::Tab:               return "Tab";
        case LexerTokenType::Newline:           return "Newline";
        case LexerTokenType::Eof:               return "Eof";

        case LexerTokenType::Unkown:            return "Unkown";
        default:
            break;
    }
    return "<<BAD TYPE>>";
}