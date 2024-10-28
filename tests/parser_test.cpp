#include "Lexer/Lexer.hpp"
#include "Parser/Parser.hpp"
#include <gtest/gtest.h>

class ParserTest : public ::testing::Test
{
  protected:
    std::unique_ptr<Parser> createParser(std::string_view input) { return std::make_unique<Parser>(input, output); }
    CompilerOutput          output;
};

// Binary Operations Tests
TEST_F(ParserTest, BinaryArithmetic)
{
    std::vector<std::pair<std::string, LexerTokenType>> ops = {{"x + 5", LexerTokenType::PlusToken},
                                                               {"x - 5", LexerTokenType::MinusToken},
                                                               {"x * 5", LexerTokenType::MultiplyToken},
                                                               {"x / 5", LexerTokenType::DivideToken}};

    for (const auto& [expr, tokenType] : ops)
    {
        auto       parser = createParser(expr);
        LexerToken token;
        parser->advanceToken(token);
        auto              node    = parser->parseStatement(token);
        const BinaryNode& binNode = static_cast<const BinaryNode&>(*node);

        EXPECT_EQ(binNode.token.type, tokenType);
        EXPECT_EQ(binNode.left->token.type, LexerTokenType::VarToken);
        EXPECT_EQ(binNode.right->token.type, LexerTokenType::IntToken);
    }
}

TEST_F(ParserTest, CompoundAssignment)
{
    auto       parser = createParser("result = x + y * z");
    LexerToken token;
    parser->advanceToken(token);
    auto              node   = parser->parseStatement(token);
    const BinaryNode& assign = static_cast<const BinaryNode&>(*node);

    EXPECT_EQ(assign.token.type, LexerTokenType::AssignToken);
    EXPECT_EQ(assign.left->token.type, LexerTokenType::VarToken);
    EXPECT_EQ(assign.left->token.value, "result");

    const BinaryNode& plus = static_cast<const BinaryNode&>(*assign.right);
    EXPECT_EQ(plus.token.type, LexerTokenType::PlusToken);
    EXPECT_EQ(plus.left->token.type, LexerTokenType::VarToken);

    const BinaryNode& mult = static_cast<const BinaryNode&>(*plus.right);
    EXPECT_EQ(mult.token.type, LexerTokenType::MultiplyToken);
    EXPECT_EQ(mult.left->token.type, LexerTokenType::VarToken);
    EXPECT_EQ(mult.right->token.type, LexerTokenType::VarToken);
}

// Print Node Tests
TEST_F(ParserTest, SimplePrint)
{
    auto       parser = createParser("print(x)");
    LexerToken token;
    parser->advanceToken(token);
    auto            node      = parser->parseStatement(token);
    const TreeNode& printNode = static_cast<const TreeNode&>(*node);

    EXPECT_EQ(printNode.token.type, LexerTokenType::PrintToken);
    EXPECT_EQ(printNode.children[0]->token.type, LexerTokenType::VarToken);
}

TEST_F(ParserTest, PrintExpression)
{
    auto       parser = createParser("print(x + y * z)");
    LexerToken token;
    parser->advanceToken(token);
    auto            node      = parser->parseStatement(token);
    const TreeNode& printNode = static_cast<const TreeNode&>(*node);

    EXPECT_EQ(printNode.token.type, LexerTokenType::PrintToken);

    const BinaryNode& plus = static_cast<const BinaryNode&>(*printNode.children[0]);
    EXPECT_EQ(plus.token.type, LexerTokenType::PlusToken);

    const BinaryNode& mult = static_cast<const BinaryNode&>(*plus.right);
    EXPECT_EQ(mult.token.type, LexerTokenType::MultiplyToken);
}

// Error Cases
TEST_F(ParserTest, MissingCondition)
{
    auto       parser = createParser("if:\n    print(x)\n");
    LexerToken token;
    parser->advanceToken(token);
    EXPECT_THROW(parser->parseStatement(token), Error);
}

TEST_F(ParserTest, MissingPrintExpression)
{
    auto       parser = createParser("print()");
    LexerToken token;
    parser->advanceToken(token);
    EXPECT_THROW(parser->parseStatement(token), Error);
}

TEST_F(ParserTest, InvalidBinaryOperation)
{
    auto       parser = createParser("x + * y");
    LexerToken token;
    parser->advanceToken(token);
    EXPECT_THROW(parser->parseStatement(token), Error);
}