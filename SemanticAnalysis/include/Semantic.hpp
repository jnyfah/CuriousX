#ifndef SEMANTIC_HPP
#define SEMANTIC_HPP

#include "SymbolTable.hpp"
#include "SyntaxAnalysis/include/Node.hpp"
#include <vector>


class Semantic
{

  public:
    Semantic() {}

    void traverse(std::vector<std::shared_ptr<Node>> compound);

    void checkAssignments(std::shared_ptr<Node> node, const std::string &inferredType);

    void checkExpr(std::shared_ptr<Node> node);

    std::optional<std::string> inferType(std::shared_ptr<Node> node);


  private:
    symbolTable::Table symboltable;
};

#endif