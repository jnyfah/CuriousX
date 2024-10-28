#include "CompilerOutput.hpp"
#include "Lexer/Lexer.hpp"
#include "Parser/Parser.hpp"
#include "Semantic/Semantic.hpp"
#include <gtest/gtest.h>


class SemanticTest : public ::testing::Test
{
  protected:
    CompilerOutput output;
    Semantic       semantic{output};

    std::unique_ptr<BinaryNode> createLeafNode(std::string_view value, LexerTokenType type)
    {
        return ASTNodeFactory::createBinaryNode(nullptr, nullptr, LexerToken{value, {0, 0}, type});
    }

    // Helper to create a token
    LexerToken createToken(std::string_view value, LexerTokenType type) { return LexerToken{value, {0, 0}, type}; }

    // Helper for binary operations
    std::unique_ptr<BinaryNode> createBinaryOperation(std::unique_ptr<BinaryNode> left,
                                                      std::unique_ptr<BinaryNode> right,
                                                      LexerTokenType              opType,
                                                      std::string_view            opValue)
    {
        return ASTNodeFactory::createBinaryNode(std::move(left), std::move(right), LexerToken{opValue, {0, 0}, opType});
    }
};

TEST_F(SemanticTest, ValidAssignment)
{
    auto left  = createLeafNode("x", LexerTokenType::VarToken);
    auto right = createLeafNode("42", LexerTokenType::IntToken);
    auto node  = ASTNodeFactory::createBinaryNode(
        std::move(left), std::move(right), createToken("=", LexerTokenType::AssignToken));

    EXPECT_NO_THROW(semantic.analyzeTree(*node));
}

TEST_F(SemanticTest, ValidArithmetic)
{
    auto left  = createLeafNode("x", LexerTokenType::VarToken);
    auto right = createLeafNode("5", LexerTokenType::IntToken);
    auto node  = createBinaryOperation(std::move(left), std::move(right), LexerTokenType::PlusToken, "+");

    EXPECT_NO_THROW(semantic.analyzeTree(*node));
}

TEST_F(SemanticTest, TypeMismatch)
{
    auto left  = createLeafNode("hello", LexerTokenType::StringToken);
    auto right = createLeafNode("42", LexerTokenType::IntToken);
    auto node  = createBinaryOperation(std::move(left), std::move(right), LexerTokenType::PlusToken, "+");

    EXPECT_THROW(semantic.analyzeTree(*node), Error);
}

TEST_F(SemanticTest, ValidComparison)
{
    auto left  = createLeafNode("x", LexerTokenType::VarToken);
    auto right = createLeafNode("5", LexerTokenType::IntToken);
    auto node  = createBinaryOperation(std::move(left), std::move(right), LexerTokenType::GreaterToken, ">");

    EXPECT_NO_THROW(semantic.analyzeTree(*node));
}

TEST_F(SemanticTest, ValidIfCondition)
{
    // Create condition: x > 5
    auto condLeft  = createLeafNode("x", LexerTokenType::VarToken);
    auto condRight = createLeafNode("5", LexerTokenType::IntToken);
    auto condition =
        createBinaryOperation(std::move(condLeft), std::move(condRight), LexerTokenType::GreaterToken, ">");

    // Create then branch: y = 1
    std::vector<std::unique_ptr<ASTNode>> thenChildren;
    auto                                  assignLeft  = createLeafNode("y", LexerTokenType::VarToken);
    auto                                  assignRight = createLeafNode("1", LexerTokenType::IntToken);
    auto                                  assignment =
        createBinaryOperation(std::move(assignLeft), std::move(assignRight), LexerTokenType::AssignToken, "=");
    thenChildren.push_back(std::move(assignment));

    auto thenBranch =
        ASTNodeFactory::createTreeNode(std::move(thenChildren), createToken("block", LexerTokenType::ProgramToken));

    auto ifNode = ASTNodeFactory::createConditionalNode(
        std::move(condition), std::move(thenBranch), nullptr, createToken("if", LexerTokenType::IfToken));

    EXPECT_NO_THROW(semantic.analyzeTree(*ifNode));
}
