#ifndef SEMANTIC_HPP
#define SEMANTIC_HPP

#include "SymbolTable.hpp"
#include "SyntaxAnalysis/include/Node.hpp"
#include <vector>


class Semantic
{

  public:
    Semantic(std::vector<std::shared_ptr<Node>> compound): compound(compound) {}

    void traverse();

    void checkAssignments(std::shared_ptr<Node> node, const InferredType &inferredType);

    void checkExpr(std::shared_ptr<Node> node);

    void checkPrint(std::shared_ptr<Node> node);

    std::optional<InferredType> inferType(std::shared_ptr<Node> node);

    void printSymbolTree();

    symbolTable::Table getSymbolTable() const;


  private:
    symbolTable::Table symboltable;
    std::vector<std::shared_ptr<Node>> compound;
};

#endif