#pragma once

#include <string_view>
#include <unordered_map>
#include <vector>

#include "CompilerOutputParser.hpp"
#include "Semantic.hpp"

class WasmGen
{
  public:
    WasmGen();

    void                                        generate(const ASTNode& rootNode);
    const std::vector<WasmInstructionWithData>& getInstructions() const;
    const std::unordered_map<std::string, int>& getLocalMap() const;

    void                process(const ASTNode& node);
    std::vector<symbol> getSymbolTables();

    // Node traversal methods
    void generateBinaryOp(const BinaryNode& node);
    void generateConditional(const ConditionalNode& node);
    void generateBlock(const TreeNode& node);

  private:
    void processBinaryOperation(const BinaryNode& node);
    void processConditionalOperation(const ConditionalNode& node);
    void processBlockOperation(const TreeNode& node);
    // Expression generation methods
    void generateExpression(const BinaryNode& node);

    // Helper methods
    bool isFloatType(const BinaryNode& node) const;
    int  getOrCreateLocalIndex(std::string_view varName);
    void addInstruction(WasmInstructionWithData instruction);

    // Data members
    std::unordered_map<std::string, int> m_locals;
    std::vector<WasmInstructionWithData> m_instructions;
    int                                  m_nextLocalIndex = 0;
    Semantic m_semantic;
};