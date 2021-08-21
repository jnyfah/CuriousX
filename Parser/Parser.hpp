#ifndef PARSER_HPP
#define PARSER_HPP

#include <array>
#include <vector>
#include <functional>

#include "../Lexer/Lexer.hpp"
#include "../utils/SourceLocation.hpp"
#include "../utils/Operations.hpp"

class Parser {

    public:

        
    private:
        unsigned m_index = 0;
        Lexer m_lexer;
        LexerToken currentToken;


};


#endif // PARSER_HPP