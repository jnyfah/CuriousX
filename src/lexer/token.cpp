#include "token.hpp"

namespace cx
{

    const char* describe(TokenType t)
    {
        switch (t)
        {
            case TokenType::ParenOpen:
                return "(";
            case TokenType::ParenClose:
                return ")";
            case TokenType::BracesOpen:
                return "{";
            case TokenType::BracesClose:
                return "}";

            case TokenType::Float:
                return "float";
            case TokenType::Int:
                return "integer";
            case TokenType::Var:
                return "identifier";
            case TokenType::String:
                return "string";
            case TokenType::Bool:
                return "boolean";

            case TokenType::If:
                return "if";
            case TokenType::Else:
                return "else";
            case TokenType::While:
                return "while";
            case TokenType::Func:
                return "func";
            case TokenType::Return:
                return "return";
            case TokenType::Print:
                return "print";

            case TokenType::Plus:
                return "+";
            case TokenType::Minus:
                return "-";
            case TokenType::Divide:
                return "/";
            case TokenType::Multiply:
                return "*";
            case TokenType::Percent:
                return "%";

            case TokenType::PlusEq:
                return "+=";
            case TokenType::MinusEq:
                return "-=";
            case TokenType::DivideEq:
                return "/=";
            case TokenType::MultiplyEq:
                return "*=";
            case TokenType::PercentEq:
                return "%=";

            case TokenType::Assign:
                return "=";
            case TokenType::Equal:
                return "==";
            case TokenType::NotEqual:
                return "!=";
            case TokenType::Less:
                return "<";
            case TokenType::LessEqual:
                return "<=";
            case TokenType::Greater:
                return ">";
            case TokenType::GreaterEqual:
                return ">=";

            case TokenType::Not:
                return "!";
            case TokenType::And:
                return "&";
            case TokenType::Or:
                return "|";

            case TokenType::Semicolon:
                return ";";
            case TokenType::Comma:
                return ",";
            case TokenType::Eof:
                return "end of file";

            case TokenType::Unknown:
                return "invalid token";
        }
        // no default: -Wswitch flags any enumerator added above but not handled here
        return "invalid token";
    }

    std::string describe(const Token& t)
    {
        if (t.value.empty())
        {
            return describe(t.type);
        }

        std::string out;
        out.reserve(t.value.size());
        for (const char c : t.value)
        {
            switch (c)
            {
                case '\n':
                    out += "\\n";
                    break;
                case '\t':
                    out += "\\t";
                    break;
                case '\r':
                    out += "\\r";
                    break;
                default:
                    out += c;
                    break;
            }
        }
        return out;
    }

} // namespace cx
