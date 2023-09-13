#ifndef SEMANTIC_HPP
#define SEMANTIC_HPP

#include "SymbolTable.hpp"
#include "SyntaxAnalysis/include/Node.hpp"
#include <vector>



class Semantic
{

  public:
    explicit Semantic(){}

    void traverse(const std::vector<std::shared_ptr<Node>> &compound);

    void checkAssignments(std::shared_ptr<Node> node, const InferredType &inferredType);

    void checkExpr(std::shared_ptr<Node> node);

    void checkPrint(std::shared_ptr<Node> node);

    std::optional<InferredType> inferType(std::shared_ptr<Node> node);

    nlohmann::json printSymbolTree();

    symbolTable::Table getSymbolTable() const;


  private:
    symbolTable::Table symboltable;
};

#endif