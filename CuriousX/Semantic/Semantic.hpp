#pragma once

#include "CompilerOutput.hpp"
#include "SymbolTable.hpp"

class Semantic
{
  public:
    Semantic(CompilerOutput& output) : m_output(output) {}

    bool analyzeTree(const ASTNode& node);
    void addSymbolTableToOutput();

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
    std::string                getVariableName(const ASTNode& node) const;
    nlohmann::json             tableToJson(const symbolTable& table);
    constexpr std::string_view getInferredTypeDescription(const InferredType& t);
    bool                       isComparisonOp(const BinaryNode& node);

    // Member variables
    bool            m_hasNonLiteral = false;
    CompilerOutput& m_output;
};