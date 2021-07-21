#ifndef PARSER_HPP
#define PARSER_HPP

#include "../Lexer/tokenizer.hpp"
#include <memory>


enum class NodeType {

    // Misc
    ERROR,

    // Declarations
    VAR_DECL,

    // Expressions
    IDENTIFIER_EXPR,
    NUMBER_EXPR,
    STRING_EXPR,

    // Statements      
    PRINT_STMT,
    
    // Operators
    ADD,
    SUBTRACT,
    DIVIDE,
    MULTIPLY,             

    unknown

};



struct Node {
        std::string value;
        NodeType type;
        Node *left;
        Node *right;
        SourceLocation location;
      };


class Parser {
    public:
       void parse(std::vector<Token> &token);

    private:
        std::unique_ptr<Node> makeNode(std::string value, NodeType type, Node *left, Node *right, SourceLocation location);
        std::unique_ptr<Node> makeLeafNode(std::string value, NodeType type, SourceLocation location);
        std::unique_ptr<Node> makeUnaryNode(std::string value, NodeType type, Node *left, SourceLocation location);
       
};


#endif