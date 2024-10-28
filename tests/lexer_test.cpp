#include "Lexer/Lexer.hpp"
#include "Parser/Parser.hpp"
#include <gtest/gtest.h>

std::vector<LexerToken> tokenize(std::string_view input)
{
    Lexer                   lexer(input);
    std::vector<LexerToken> tokens;
    for (auto token = lexer.nextNWToken(); token.type != LexerTokenType::Eof; token = lexer.nextNWToken())
    {
        tokens.push_back(token);
    }
    return tokens;
}

void expectToken(const LexerToken& token, LexerTokenType expectedType, std::string_view expectedValue)
{
    EXPECT_EQ(token.type, expectedType);
    EXPECT_EQ(token.value, expectedValue);
}

class LexerTest : public ::testing::Test
{
  protected:
    void verifyTokenSequence(const std::vector<LexerToken>&                             tokens,
                             const std::vector<std::pair<LexerTokenType, std::string>>& expected)
    {
        ASSERT_EQ(tokens.size(), expected.size());
        for (size_t i = 0; i < tokens.size(); ++i)
        {
            expectToken(tokens[i], expected[i].first, expected[i].second);
        }
    }
};

TEST_F(LexerTest, Numbers)
{
    auto tokens = tokenize(R"(42 3.14 -17 -2.5 "hello" true #comment)");

    std::vector<std::pair<LexerTokenType, std::string>> expected = {
        {LexerTokenType::IntToken, "42"},
        {LexerTokenType::FloatToken, "3.14"},
        {LexerTokenType::MinusToken, "-"},
        {LexerTokenType::IntToken, "17"},
        {LexerTokenType::MinusToken, "-"},
        {LexerTokenType::FloatToken, "2.5"},
        {LexerTokenType::StringToken, "\"hello\""},
        {LexerTokenType::BoolToken, "true"},
        {LexerTokenType::CommentToken, "#comment"},
    };

    verifyTokenSequence(tokens, expected);
}

TEST_F(LexerTest, Operators)
{
    auto tokens = tokenize("+-*/ == != < <= > >=");

    std::vector<std::pair<LexerTokenType, std::string>> expected = {
        {LexerTokenType::PlusToken, "+"},
        {LexerTokenType::MinusToken, "-"},
        {LexerTokenType::MultiplyToken, "*"},
        {LexerTokenType::DivideToken, "/"},
        {LexerTokenType::EqualToken, "=="},
        {LexerTokenType::NotEqualToken, "!="},
        {LexerTokenType::LessToken, "<"},
        {LexerTokenType::LessEqualToken, "<="},
        {LexerTokenType::GreaterToken, ">"},
        {LexerTokenType::GreaterEqualToken, ">="},
    };

    verifyTokenSequence(tokens, expected);
}

TEST_F(LexerTest, Keywords)
{

    auto tokens = tokenize(R"(
x = 42
if (x >= 0){
y = x + 10
    print(y)
})");

    std::vector<std::pair<LexerTokenType, std::string>> expected = {
        {LexerTokenType::Newline, "\\n"},          {LexerTokenType::VarToken, "x"},
        {LexerTokenType::AssignToken, "="},        {LexerTokenType::IntToken, "42"},
        {LexerTokenType::Newline, "\\n"},          {LexerTokenType::IfToken, "if"},
        {LexerTokenType::ParenOpen, "("},          {LexerTokenType::VarToken, "x"},
        {LexerTokenType::GreaterEqualToken, ">="}, {LexerTokenType::IntToken, "0"},
        {LexerTokenType::ParenClose, ")"},         {LexerTokenType::BracesOpen, "{"},
        {LexerTokenType::Newline, "\\n"},          {LexerTokenType::VarToken, "y"},
        {LexerTokenType::AssignToken, "="},        {LexerTokenType::VarToken, "x"},
        {LexerTokenType::PlusToken, "+"},          {LexerTokenType::IntToken, "10"},
        {LexerTokenType::Newline, "\\n"},          {LexerTokenType::PrintToken, "print"},
        {LexerTokenType::ParenOpen, "("},          {LexerTokenType::VarToken, "y"},
        {LexerTokenType::ParenClose, ")"},         {LexerTokenType::Newline, "\\n"},
        {LexerTokenType::BracesClose, "}"},
    };

    verifyTokenSequence(tokens, expected);
}
