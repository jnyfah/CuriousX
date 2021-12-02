#ifndef _PARSER_H_
#define _PARSER_H_


#include "../Lexer/Lexer.hpp"
#include "Tree.hpp"

class Parser {
private:
    Lexer lexer;
    LexerTokenType token;

public:
    Parser(Lexer lexer);
    LexerToken *parser_eat(LexerTokenType type);
    ASTree *parser_parse();
    ASTree *parser_parse_block();
    ASTree *parser_parse_expression();
    ASTree *parser_parse_int();
    ASTree *parser_parse_string();
    ASTree *parser_parse_statement();
    ASTree *parser_parse_compound();
    ASTree *parser_parse_definition();
};


#endif