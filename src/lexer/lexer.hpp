#pragma once
#include <optional>
#include <string_view>
#include "token.hpp"
#include "tools/diagnostics.hpp"

namespace cx
{

    class Lexer
    {
    public:
        explicit Lexer(std::string_view data, Diagnostics& diag);
        Token nextToken();

    private:
        Token            doGetNextToken();
        char             next_char();
        char             peek_next_char() const;
        Location         currentLocation() const;
        bool             match(char expected);

        bool             isAlpha(char c);
        bool             isDigit(char c);
        Token            lexNumber(size_t start, Location loc);
        Token            badNumber(size_t start, Location loc, std::string_view what);

        Token            lexIdentifier(size_t start, Location loc);
        Token            lexString(Location loc);
        void             skipTrivia();

        std::string_view m_data;
        size_t           m_pos = 0;
        unsigned short   x_pos = 1, y_pos = 1;
        Diagnostics&     m_diag;
    };

} // namespace cx