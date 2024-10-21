#pragma once

#include "SymbolTable.hpp"

class Semantic
{
  public:
    Semantic() {}

    symbolTable getSymbolTables();
    bool analyze(const ASTNode& node);

  private:
    
    // Analysis methods
    void analyzeBinaryOperation(const BinaryNode& node);
    void analyzeConditionalOperation(const ConditionalNode& node);
    void analyzeBlockOperation(const TreeNode& node);
    void analyzeAssignment(const BinaryNode& node);
    void analyzeExpression(const BinaryNode& node);

    // Type inference methods
    InferredType inferType(const ASTNode& node);
    InferredType inferTypeFromVariable(const ASTNode& node);
    InferredType inferTypeFromOperation(const BinaryNode& node);

    // Validation methods
    void checkDivisionByZero(const ASTNode& node);
    bool isValidConditionType(const LexerToken& token) const;
    bool isValidBinaryType(const LexerToken& token) const;
    void ensureTypeMatch(InferredType left, InferredType right, const LexerToken& token) const;

    // Helper methods
    std::string getVariableName(const ASTNode& node) const;

    // Member variables
    bool    m_hasAnalyzedExpression = false;
};