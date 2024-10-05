#pragma once

#include "SymbolTable.hpp"
#include <vector>

class Semantic
{

  public:
    explicit Semantic() : m_symboltable(), flag(false) {}

    bool analyze(const ASTNode& node);
    void analyzeBinaryOperation(const BinaryNode& node);
    void analyzeAssignment(const BinaryNode& node);
    void analyzeExpression(const BinaryNode& node);
    void checkDivisionByZero(const ASTNode& node);
    InferredType inferType(const ASTNode& node);
    InferredType inferTypeFromVariable(const ASTNode& node);
    InferredType inferTypeFromOperation(const BinaryNode& node);
    InferredType inferTypeFromCondition(const BinaryNode& node);
    void analyzeConditionalOperation(const ConditionalNode& node);
    bool isValidConditionType(const LexerToken& type);
    void analyzeBlockOperation(const TreeNode& node);
    bool isValidBinaryType(const LexerToken& token);

  private:
    ScopedSymbolTable m_symboltable;
    bool flag;
};
