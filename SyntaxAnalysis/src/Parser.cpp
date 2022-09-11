#include "Parser.hpp"


namespace Parser{

std::unique_ptr<Node> Parser::Factor() {

    auto token = lexer.nextNWToken();
    std::unique_ptr<Node> node;

    if( (token.type == LexerTokenType::IntToken) || (token.type == LexerTokenType::FloatToken) || (token.type == LexerTokenType::VarToken) ) {  
        node = makeLeaf(token);
        return node;
    } else if(token.type == LexerTokenType::ParenOpen) {
        lexer.nextNWToken();
        node = Expression();

        if(lexer.nextNWToken().type == LexerTokenType::ParenClose) {
            lexer.nextNWToken();
            return node;  // next token
        }
    } else if(token.type == LexerTokenType::LetToken) {
        node = Assign();
        return node;
    }
 // print error
}


std::unique_ptr<Node> Parser::Assign() {
    auto token = lexer.nextNWToken();
    std::unique_ptr<Node> node;

    if(token.type == LexerTokenType::VarToken) {
        // node append
        if(lexer.nextNWToken().type == LexerTokenType::AssignToken) {
            node = Expression();
            return node;
        }
    }

    // Print error
}

bool Parser::Parser::Parse() {
    root = Expression();
}



std::unique_ptr<Node>Parser::Expression() {
        
}

};