#ifndef OPERATIONS_HPP
#define OPERATIONS_HPP

#include "SourceLocation.hpp"
#include <vector>

    /**
        * enum  Operation
        * brief Operations available in Kalculator
    */

enum class Operations {

    ASSIGNMENT,
    DIVIDE,
    EQUAL,
    PLUS,
    MINUS,
    MULTIPLY,

    UNKOWN_OP
        
};


enum class NodeKind : unsigned int
            {
                // Misc
                ERROR,

                // Declarations
                VAR_DECL,

                // Expressions
                ARRAY_EXPR,
                BINARY_EXPR,
                BOOLEAN_EXPR,
                CALL_EXPR,
                IDENTIFIER_EXPR,
                NUMBER_EXPR,
                STRING_EXPR,

                // Statements
                COMPOUND_STMT,
                PRINT_STMT,
               
    };

struct Node {
    NodeKind nodetype;                 
    std::string name;              
    std::vector<std::unique_ptr<Node>> children;       /*!< List of children nodes linked to this AST node */
    SourceLocation location;  
};



#endif