#pragma once

#include <unordered_map>
#include <vector>

#include "CompilerOutputParser.hpp"

class WasmGen
{
  public:
    WasmGen() : localVarIndex(0) {}
    void traverse(const ASTNode& node);
    bool isFloatType(const BinaryNode& node);

    void generateBinaryOp(const BinaryNode& node);
    void generateConditional(const ConditionalNode& node);
    void generateBlock(const TreeNode& node);
    void generateExpression(const BinaryNode& node);

    int getLocalIndex(std::string_view varName);

    void addInstruction(const WasmInstructionWithData& instruction);
    const std::vector<WasmInstructionWithData>& getInstructions() const;
    const std::unordered_map<std::string, int>& getLocalMap() const;

  private:
    std::unordered_map<std::string, int> locals;
    int localVarIndex;
    std::vector<WasmInstructionWithData> instructions;
};
