#include "token.hpp"

namespace cx
{

    const char* toString(const TokenType& t)
    {
        switch (t)
        {
            case TokenType::ParenOpen:
                return "ParenOpen";
            case TokenType::ParenClose:
                return "ParenClose";
            case TokenType::BracesOpen:
                return "BracesOpen";
            case TokenType::BracesClose:
                return "BracesClose";

            case TokenType::Float:
                return "Float";
            case TokenType::Int:
                return "Int";
            case TokenType::Var:
                return "Var";
            case TokenType::String:
                return "String";

            case TokenType::If:
                return "If";
            case TokenType::Else:
                return "Else";
            case TokenType::Bool:
                return "Bool";
            case TokenType::While:
                return "While";
            case TokenType::For:
                return "For";

            case TokenType::Plus:
                return "Plus";
            case TokenType::PlusEq:
                return "PlusEq";
            case TokenType::Minus:
                return "Minus";
            case TokenType::MinusEq:
                return "MinusEq";
            case TokenType::Divide:
                return "Divide";
            case TokenType::DivideEq:
                return "DivideEq";
            case TokenType::Multiply:
                return "Multiply";
            case TokenType::MultiplyEq:
                return "MultiplyEq";
            case TokenType::Assign:
                return "Assign";
            case TokenType::Print:
                return "Print";

            case TokenType::Or:
                return "Or";
            case TokenType::And:
                return "And";
            case TokenType::Percent:
                return "Percent";
            case TokenType::PercentEq:
                return "PercentEq";

            case TokenType::Func:
                return "Func";
            case TokenType::Return:
                return "Return";
            case cx::TokenType::Not:
                return "Not";
            case TokenType::Equal:
                return "Equal";
            case TokenType::NotEqual:
                return "NotEqual";
            case TokenType::LessEqual:
                return "LessEqual";
            case TokenType::Less:
                return "Less";
            case TokenType::GreaterEqual:
                return "GreaterEqual";
            case TokenType::Greater:
                return "Greater";

            case TokenType::Comment:
                return "Comment";
            case TokenType::Comma:
                return "Comma";
            case TokenType::Space:
                return "Space";
            case TokenType::Tab:
                return "Tab";
            case TokenType::Newline:
                return "Newline";
            case TokenType::Eof:
                return "Eof";

            case TokenType::Unknown:
                return "Unknown";
            default:
                break;
        }
        return "<<BAD TYPE>>";
    }

} // namespace cx