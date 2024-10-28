#pragma once

#include <string_view>
#include <unordered_map>
#include <vector>

#include "Semantic.hpp"
#include "WasmInstructions.hpp"


class WasmGen
{
  public:
    WasmGen(CompilerOutput& output): m_output(output){}

    void                                        generate(const ASTNode& rootNode);
    void addGeneratedCodeToOutput();
    const std::vector<WasmInstructionWithData> getInstructions() const;

  private:
    // Node traversal methods
    void generateBinaryOp(const BinaryNode& node);
    void generateConditional(const ConditionalNode& node);
    void generateBlock(const TreeNode& node);
    // Expression generation methods
    void generateExpression(const BinaryNode& node);

    // Helper methods
    bool isFloatType(const BinaryNode& node);
    int  getOrCreateLocalIndex(std::string_view varName);
    void addInstruction(WasmInstructionWithData instruction);

    // Data members
    std::unordered_map<std::string, int> m_locals;
    std::vector<WasmInstructionWithData> m_instructions;
    int                                  m_nextLocalIndex = 0;
    int                                  m_stringOffset = 0;
    CompilerOutput&        m_output;
};