#pragma once

#include <unordered_map>
#include <vector>


#include "CompilerOutputParser.hpp"

class WasmGen
{
  public:
    // Constructor
    WasmGen(): localVarIndex(0){}

    // Main traversal function
    void traverse(const ASTNode& node);
    bool isFloatType(const BinaryNode& node);

    // Generation functions for different node types
    void generateBinaryOp(const BinaryNode& node);
    void generateConditional(const ConditionalNode& node);
    void generateBlock(const TreeNode& node);

    void generateExpression(const BinaryNode& node);

    // Variable management
    int getLocalIndex(std::string_view varName);

    // Instruction management
    void addInstruction(const WasmInstructionWithData& instruction);
    const std::vector<WasmInstructionWithData>& getInstructions() const;
    const std::unordered_map<std::string, int>& getLocalMap() const;

  private:
    std::unordered_map<std::string, int> locals; 
    int localVarIndex; 
    std::vector<WasmInstructionWithData> instructions;
};

// std::string generateWAT(const std::vector<WasmInstructionWithData>& instructions)
// {
//     std::string wat_output;
//     for (const auto& instr : instructions)
//     {
//         wat_output += instructionToString(instr) + "\n";
//     }
//     return wat_output;
// }

// // Example usage
// std::string wat_code = generateWAT(instructions);
// std::cout << wat_code;
