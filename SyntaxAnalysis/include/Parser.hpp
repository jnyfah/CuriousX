#ifndef PARSER_HPP
#define PARSER_HPP



#include "LexicalAnalysis/include/Lexer.hpp"
#include "Node.hpp"


namespace Parser{

class Parser {

   
public:

    Parser(Lexer::Lexer lexer): lexer(lexer){}

    std::unique_ptr<Node> Expression();
    
    std::unique_ptr<Node> Factor();

    std::unique_ptr<Node> Assign();

    bool Parse();



private:
    Lexer::Lexer lexer;
    std::unique_ptr<Node> root;
  
};

 
};

#endif