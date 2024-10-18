#pragma once

#include "CompilerOutputParser.hpp"
#include <string>
#include <unordered_map>
#include <vector>

using symbol = std::unordered_map<std::string, SymbolInfo>;
class Semantic
{
  public:
    Semantic() = default;

    bool                analyze(const ASTNode& node);
    std::vector<symbol> getSymbolTables();
    void                analyzeBinaryOperation(const BinaryNode& node);
    void                analyzeConditionalOperation(const ConditionalNode& node);
    void                analyzeBlockOperation(const TreeNode& node);

  private:
    // Analysis methods
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
    bool m_hasAnalyzedExpression = false;
};