#ifndef SEMANTIC_HPP
#define SEMANTIC_HPP

#include <vector>
#include "SymbolTable.hpp"
#include "SyntaxAnalysis/include/Node.hpp"


class Semantic {

    public:
        Semantic(){}

        void traverse(std::vector<std::shared_ptr<Node>> compound);

        void checkAssignments(std::shared_ptr<Node> node);

        void checkExpr(std::shared_ptr<Node> node);


    private:
        symbolTable::Table symboltable;
        

};

#endif