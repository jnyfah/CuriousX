#pragma once

#include "CompilerOutput.hpp"
#include "SymbolTable.hpp"

class Semantic
{
  public:
    Semantic(CompilerOutput& output) : m_output(output) {}

    void analyzeTree(const ASTNode& node);
    void addSymbolTableToOutput();

  private:
    // Analysis methods
    void analyzeBinaryOperation(const BinaryNode& node);
    void analyzeConditionalOperation(const ConditionalNode& node);
    void analyzeBlockOperation(const TreeNode& node);
    void analyzeAssignment(const BinaryNode& node);
    void analyzeExpression(const BinaryNode& node);
    void analyzePrintOperation(const TreeNode& node);
    void analyzePrintExpression(const ASTNode& node);
    

    // Type inference methods
    InferredType inferType(const ASTNode& node);
    InferredType inferTypeFromVariable(const ASTNode& node);
    InferredType inferTypeFromOperation(const BinaryNode& node);

    // Validation methods
    void checkDivisionByZero(const ASTNode& node);
    bool isValidConditionType(const LexerToken& token) const;
    bool isValidBinaryType(const LexerToken& token) const;
    bool containsNonLiteral(const ASTNode& node) const;
    bool isSimpleLiteralOrVariable(const ASTNode& node) const;
    void ensureTypeMatch(InferredType left, InferredType right, const LexerToken& token) const;

    // Helper methods
    std::string                getVariableName(const ASTNode& node) const;
    nlohmann::json             tableToJson(const symbolTable& table);
    constexpr std::string_view getInferredTypeDescription(const InferredType& t);
    bool                       isComparisonOp(const BinaryNode& node);

    // Member variables
    CompilerOutput& m_output;
};