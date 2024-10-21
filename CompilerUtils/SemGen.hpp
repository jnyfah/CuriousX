// // ASTPass.hpp
// #pragma once
// #include "CodeGen.hpp"
// #include "Semantic.hpp"


// class ASTPass
// {
//   public:
//     ASTPass(Semantic& semantic, WasmGen& gen) : m_semantic(semantic), m_gen(gen) {}

//     void traverse(const ASTNode& node)
//     {
//         switch (node.getType())
//         {
//         case NodeType::BinaryOperation:
//             processBinaryOperation(static_cast<const BinaryNode&>(node));
//             break;
//         case NodeType::ConditionalOperation:
//             processConditionalOperation(static_cast<const ConditionalNode&>(node));
//             break;
//         case NodeType::BlockOperation:
//             processBlockOperation(static_cast<const TreeNode&>(node));
//             break;
//         default:
//             throw std::runtime_error("Unexpected node type");
//         }
//     }

//   private:
//     void processBinaryOperation(const BinaryNode& node)
//     {
//         m_semantic.analyzeBinaryOperation(node); // Validate
//         m_gen.generateBinaryOp(node);
//     }
//     void processConditionalOperation(const ConditionalNode& node)
//     {
//         m_semantic.analyzeConditionalOperation(node); // Validate
//         m_gen.generateConditional(node);
//     }
//     void processBlockOperation(const TreeNode& node)
//     {
//         m_semantic.analyzeBlockOperation(node); // Validate
//         m_gen.generateBlock(node);
//     }

//     Semantic& m_semantic;
//     WasmGen&  m_gen;
// };
