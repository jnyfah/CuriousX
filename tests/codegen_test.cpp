#include <gtest/gtest.h>
#include "Lexer/Lexer.hpp"
#include "Parser/Parser.hpp"
#include "Semantic/Semantic.hpp"
#include "CompilerOutput.hpp"
#include "Generation/Codegen.hpp"
#include <iostream>


class WasmGenTest : public ::testing::Test {
protected:
    CompilerOutput output;
    std::unique_ptr<WasmGen> generator;

    void SetUp() override {
        generator = std::make_unique<WasmGen>(output);  // Pass output to constructor
    }

    // Helper for creating leaf nodes
    std::unique_ptr<BinaryNode> createLeafNode(std::string_view value, LexerTokenType type) {
        return ASTNodeFactory::createBinaryNode(
            nullptr,
            nullptr,
            LexerToken{value, {0, 0}, type}
        );
    }

    void verifyInstructions(const std::vector<WasmInstructionWithData>& actual,
                          const std::vector<std::pair<WasmInstruction, std::string>>& expected) {
        ASSERT_EQ(actual.size(), expected.size()) << "Instruction count mismatch";
        
        for (size_t i = 0; i < actual.size(); ++i) {
            EXPECT_EQ(actual[i].instruction, expected[i].first)
                << "Instruction mismatch at position " << i;
            if (!expected[i].second.empty()) {
                EXPECT_TRUE(actual[i].has_data)
                    << "Missing data at position " << i;
                EXPECT_EQ(actual[i].data, expected[i].second)
                    << "Data mismatch at position " << i 
                    << "\nExpected: '" << expected[i].second 
                    << "'\nActual: '" << actual[i].data << "'";
            }
        }
    }
};

TEST_F(WasmGenTest, IntegerArithmetic) {
    // Create: 5 + 3
    auto left = createLeafNode("5", LexerTokenType::IntToken);
    auto right = createLeafNode("3", LexerTokenType::IntToken);
    auto node = ASTNodeFactory::createBinaryNode(
        std::move(left),
        std::move(right),
        LexerToken{"+", {0, 0}, LexerTokenType::PlusToken}
    );

    generator->generate(*node);
    auto instructions = generator->getInstructions();

    std::vector<std::pair<WasmInstruction, std::string>> expected = {
        {WasmInstruction::I32Const, "5"},
        {WasmInstruction::I32Const, "3"},
        {WasmInstruction::I32Add, ""}
    };
    verifyInstructions(instructions, expected);
}

TEST_F(WasmGenTest, FloatArithmetic) {
    auto left = createLeafNode("5.0", LexerTokenType::FloatToken);
    auto right = createLeafNode("3.0", LexerTokenType::FloatToken);
    auto node = ASTNodeFactory::createBinaryNode(
        std::move(left),
        std::move(right),
        LexerToken{"*", {0, 0}, LexerTokenType::MultiplyToken}
    );

    generator->generate(*node);
    auto instructions = generator->getInstructions();

    std::vector<std::pair<WasmInstruction, std::string>> expected = {
        {WasmInstruction::F32Const, "5.0"},
        {WasmInstruction::F32Const, "3.0"},
        {WasmInstruction::F32Mul, ""}
    };
    verifyInstructions(instructions, expected);
}

TEST_F(WasmGenTest, VariableAssignment) {
    auto left = createLeafNode("x", LexerTokenType::VarToken);
    auto right = createLeafNode("42", LexerTokenType::IntToken);
    auto node = ASTNodeFactory::createBinaryNode(
        std::move(left),
        std::move(right),
        LexerToken{"=", {0, 0}, LexerTokenType::AssignToken}
    );

    generator->generate(*node);
    auto instructions = generator->getInstructions();

    std::vector<std::pair<WasmInstruction, std::string>> expected = {
        {WasmInstruction::I32Const, "42"},
        {WasmInstruction::LocalSet, "0"}
    };
    verifyInstructions(instructions, expected);
}

TEST_F(WasmGenTest, IfStatement) {
    // Create: if x > 5: print(x)
    auto condition = ASTNodeFactory::createBinaryNode(
        createLeafNode("x", LexerTokenType::VarToken),
        createLeafNode("5", LexerTokenType::IntToken),
        LexerToken{">", {0, 0}, LexerTokenType::GreaterToken}
    );

    std::vector<std::unique_ptr<ASTNode>> thenChildren;
    thenChildren.push_back(createLeafNode("x", LexerTokenType::VarToken));
    auto thenBranch = ASTNodeFactory::createTreeNode(
        std::move(thenChildren),
        LexerToken{"print", {0, 0}, LexerTokenType::PrintToken}
    );

    auto ifNode = ASTNodeFactory::createConditionalNode(
        std::move(condition),
        std::move(thenBranch),
        nullptr,
        LexerToken{"if", {0, 0}, LexerTokenType::IfToken}
    );

    generator->generate(*ifNode);
    auto instructions = generator->getInstructions();

    std::vector<std::pair<WasmInstruction, std::string>> expected = {
        {WasmInstruction::LocalGet, "0"},
        {WasmInstruction::I32Const, "5"},
        {WasmInstruction::I32GtS, ""},
        {WasmInstruction::If, ""},
        {WasmInstruction::LocalGet, "0"},
        {WasmInstruction::CallPrint, ""},
        {WasmInstruction::End, ""}
    };
    verifyInstructions(instructions, expected);
}
