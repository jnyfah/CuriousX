#pragma once

#include "CompilerOutputParser.hpp"
#include <vector>

class Semantic
{

  public:
    explicit Semantic() : m_symboltable(), flag(false) {}

    bool analyze(const ASTNode& node);

    void analyzeAssignment(const BinaryNode& node);
    void analyzeExpression(const BinaryNode& node);
    void analyzeBinaryOperation(const BinaryNode& node);
    void analyzeConditionalOperation(const ConditionalNode& node);

    void checkDivisionByZero(const ASTNode& node);

    InferredType inferType(const ASTNode& node);
    InferredType inferTypeFromVariable(const ASTNode& node);
    InferredType inferTypeFromOperation(const BinaryNode& node);
    InferredType inferTypeFromCondition(const BinaryNode& node);

    bool isValidConditionType(const LexerToken& type);
    void analyzeBlockOperation(const TreeNode& node);
    bool isValidBinaryType(const LexerToken& token);

    const std::vector<std::unordered_map<std::string, SymbolInfo>> getSymbolTable();

  private:
    ScopedSymbolTable m_symboltable;
    bool flag;
};
