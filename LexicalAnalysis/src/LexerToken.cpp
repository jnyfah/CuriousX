#include "LexerToken.hpp"

const char *toString(LexerTokenType t)
{
  switch (t) {
  case LexerTokenType::ParenOpen:
    return "ParenOpen";
  case LexerTokenType::ParenClose:
    return "ParenClose";
  case LexerTokenType::BraceOpen:
    return "BraceOpen";
  case LexerTokenType::BraceClose:
    return "BraceClose";
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

  case LexerTokenType::IfToken:
    return "IfToken";
  case LexerTokenType::ElseToken:
    return "ElseToken";
  case LexerTokenType::NotToken:
    return "NotToken";
  case LexerTokenType::StringToken:
    return "StringToken";

  case LexerTokenType::GreaterThanToken:
    return "GreaterThanToken";
  case LexerTokenType::LessThanToken:
    return "LessThanToken";
  case LexerTokenType::GreaterEqualToken:
    return "GreaterEqualToken";
  case LexerTokenType::LessEqualToken:
    return "LessEqualToken";
  case LexerTokenType::EqualToken:
    return "EqualToken";
  case LexerTokenType::NotEqualToken:
    return "NotEqualToken";

  case LexerTokenType::SemicolonToken:
    return "Semicolon";
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