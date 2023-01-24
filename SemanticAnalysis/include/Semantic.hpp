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

    void checkAssignments(std::shared_ptr<Node> node, const InferredType &inferredType);

    void checkExpr(std::shared_ptr<Node> node);

    std::optional<InferredType> inferType(std::shared_ptr<Node> node);


  private:
    symbolTable::Table symboltable;
};

#endif