#include "lexer.hpp"
#include <algorithm>
#include <string_view>
#include <unordered_map>

namespace cx
{

    Lexer::Lexer(std::string_view data, Diagnostics& diag) : m_data(data), m_diag(diag) {}

    Token Lexer::nextNWToken()
    {
        Token t;
        do
        {
            t = nextToken();
        } while (t.type == TokenType::Space || t.type == TokenType::Tab);
        return t;
    }

    Token Lexer::nextToken()
    {
        return doGetNextToken();
    }

    char Lexer::next_char()
    {
        if (m_pos >= m_data.size())
        {
            return '\0';
        }
        char c = m_data[m_pos++];
        if (c == '\n')
        {
            y_pos++;
            x_pos = 1;
        }
        else
        {
            x_pos++;
        }
        return c;
    }

    char Lexer::peek_next_char() const
    {
        return m_pos < m_data.size() ? m_data[m_pos] : '\0';
    }

    Location Lexer::currentLocation() const
    {
        return Location(y_pos, x_pos);
    }

    bool Lexer::isAlpha(char c)
    {
        return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
    }

    bool Lexer::isDigit(char c)
    {
        return '0' <= c && c <= '9';
    }

    Token Lexer::lexNumber(size_t start, Location loc)
    {
        // integer part
        while (isDigit(peek_next_char()))
        {
            next_char();
        }

        bool isFloat = false;

        // fractional part
        if (peek_next_char() == '.')
        {
            isFloat = true;
            next_char(); // consume '.'

            if (!isDigit(peek_next_char())) // "1."  or  "1.x"
            {
                return badNumber(start, loc, "expected a digit after '.'");
            }

            while (isDigit(peek_next_char()))
            {
                next_char();
            }
        }

        // nothing identifier-ish may follow: 123abc, 1.2.3, 1_0
        const char c = peek_next_char();
        if (isAlpha(c) || c == '_' || c == '.')
        {
            return badNumber(start, loc, "invalid number literal");
        }

        return {m_data.substr(start, m_pos - start), loc, isFloat ? TokenType::Float : TokenType::Int};
    }

    Token Lexer::badNumber(size_t start, Location loc, std::string_view what)
    {
        while (isDigit(peek_next_char()) || isAlpha(peek_next_char()) || peek_next_char() == '_' || peek_next_char() == '.')
        {
            next_char();
        }

        const auto text = m_data.substr(start, m_pos - start);
        m_diag.error(loc, "{} in '{}'", what, text);
        return {text, loc, TokenType::Unknown};
    }

    Token Lexer::lexIdentifier(size_t start, Location loc)
    {
        while (isAlpha(peek_next_char()) || isDigit(peek_next_char()) || peek_next_char() == '_')
        {
            next_char();
        }

        const auto                                                   text     = m_data.substr(start, m_pos - start);

        static const std::unordered_map<std::string_view, TokenType> keywords = {
            {    "if",     TokenType::If},
            {  "else",   TokenType::Else},
            { "while",  TokenType::While},
            {   "for",    TokenType::For},
            {  "func",   TokenType::Func},
            {"return", TokenType::Return},
            { "print",  TokenType::Print},
            {  "true",   TokenType::Bool},
            { "false",   TokenType::Bool},
        };

        if (auto it = keywords.find(text); it != keywords.end())
        {
            return {text, loc, it->second};
        }

        return {text, loc, TokenType::Var};
    }

    Token Lexer::lexString(Location loc)
    {
        const size_t contentStart = m_pos;

        while (peek_next_char() != '"')
        {
            const char c = peek_next_char();
            if (c == '\0' || c == '\n')
            {
                m_diag.error(loc, "unterminated string literal");
                return {m_data.substr(contentStart, m_pos - contentStart), loc, TokenType::Unknown};
            }
            next_char();
        }

        const auto text = m_data.substr(contentStart, m_pos - contentStart);
        next_char(); // consume '"'
        return {text, loc, TokenType::String};
    }

    Token Lexer::lexComment(size_t startPos, const Location& location)
    {
        while (true)
        {
            char c = peek_next_char();
            if (c == '\n' || c == '\0')
            {
                break;
            }
            next_char();
        }
        return {m_data.substr(startPos, m_pos - startPos), location, TokenType::Comment};
    }

    bool Lexer::match(char expected)
    {
        if (peek_next_char() != expected)
        {
            return false;
        }
        next_char();
        return true;
    }

    Token Lexer::doGetNextToken()
    {
        while (peek_next_char() == ' ' || peek_next_char() == '\t')
        {
            next_char();
        }
        const Location loc   = currentLocation();
        const auto     start = m_pos;
        const char     nchar = next_char();

        switch (nchar)
        {
            case '\0':
                return {"", loc, TokenType::Eof};
            case '(':
                return {"(", loc, TokenType::ParenOpen};
            case ';':
                return {";", loc, TokenType::Semicolon};
            case ')':
                return {")", loc, TokenType::ParenClose};
            case '%':
                return {"%", loc, TokenType::Percent};
            case '&':
                return {"&", loc, TokenType::And};
            case '{':
                return {"{", loc, TokenType::BracesOpen};
            case '}':
                return {"}", loc, TokenType::BracesClose};
            case '|':
                return {"|", loc, TokenType::Or};
            case '\n':
                return {"\n", loc, TokenType::Newline};
            case '\t':
                return {"\t", loc, TokenType::Tab};
            case '>':
                return match('=') ? Token{m_data.substr(start, 2), loc, TokenType::GreaterEqual} : Token{">", loc, TokenType::Greater};
            case '<':
                return match('=') ? Token{m_data.substr(start, 2), loc, TokenType::LessEqual} : Token{"<", loc, TokenType::Less};
            case '=':
                return match('=') ? Token{m_data.substr(start, 2), loc, TokenType::Equal} : Token{"=", loc, TokenType::Assign};
            case '!':
                return match('=') ? Token{m_data.substr(start, 2), loc, TokenType::NotEqual} : Token{"!", loc, TokenType::Not};
            case '+':
                return match('=') ? Token{m_data.substr(start, 2), loc, TokenType::PlusEq} : Token{"+", loc, TokenType::Plus};
            case '-':
                return match('=') ? Token{m_data.substr(start, 2), loc, TokenType::MinusEq} : Token{"-", loc, TokenType::Minus};
            case '*':
                return match('=') ? Token{m_data.substr(start, 2), loc, TokenType::MultiplyEq} : Token{"*", loc, TokenType::Multiply};
            case '/':
                return match('=') ? Token{m_data.substr(start, 2), loc, TokenType::DivideEq} : Token{"/", loc, TokenType::Divide};
            case '"':
                return lexString(loc);
            case '#':
                return lexComment(start, loc);
            case ',':
                return {",", loc, TokenType::Comma};
            default:
                if (isDigit(nchar))
                {
                    return lexNumber(start, loc);
                }
                if (isAlpha(nchar) || nchar == '_')
                {
                    return lexIdentifier(start, loc);
                }
                m_diag.error(loc, "unknown character '{}'", nchar);
                return {m_data.substr(start, 1), loc, TokenType::Unknown};
        }
    }

} // namespace cx