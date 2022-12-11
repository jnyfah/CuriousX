#include "gtest/gtest.h"

#include "FileHandler.hpp"
#include "LexicalAnalysis/include/Lexer.hpp"
#include "SyntaxAnalysis/include/Parser.hpp"

#ifdef __APPLE__
namespace fs = std::__fs::filesystem;
#else
namespace fs = std::filesystem;
#endif

void LexerEquality(std::vector<LexerToken> actual, std::vector<LexerToken> expected)
{
    EXPECT_EQ(actual.size(), expected.size());
    for (int i = 0; i < actual.size(); i++)
    {
        EXPECT_EQ(actual[i].type, expected[i].type);
        EXPECT_EQ(actual[i].value, expected[i].value);
        EXPECT_EQ(actual[i].location.toString(), expected[i].location.toString());
    }
}

void FileEquality(std::string actuals, std::string expecteds)
{
    std::ifstream actual(actuals), expected(expecteds);

    EXPECT_EQ(fs::file_size(fs::path(actuals)), fs::file_size(fs::path(expecteds)));
    std::string lineA, lineB;
    while (std::getline(actual, lineA))
    {
        while (std::getline(expected, lineB))
        {
            EXPECT_EQ(lineA, lineB);
        }
    }
}


TEST(curiousTest, VariableAssign)
{
    std::string actual = "num1 = 1.5";

    std::vector<LexerToken> expected = {{LexerToken{"num1", {1, 1}, LexerTokenType::VarToken}},
                                        {LexerToken{"=", {1, 6}, LexerTokenType::AssignToken}},
                                        {LexerToken{"1.5", {1, 8}, LexerTokenType::FloatToken}}

    };
    Lexer lex(actual);
    std::vector<LexerToken> m_tokens;
    for (auto token = lex.nextNWToken(); token.type != LexerTokenType::Eof; token = lex.nextNWToken())
    {
        m_tokens.emplace_back(token);
    }
    LexerEquality(m_tokens, expected);
}


TEST(curiousTest, PrintData)
{
    std::string actual = "print(#The sum of num1 num2 # sum)";

    std::vector<LexerToken> expected = {{LexerToken{"print", {1, 1}, LexerTokenType::PrintToken}},
                                        {LexerToken{"(", {1, 6}, LexerTokenType::ParenOpen}},
                                        {LexerToken{"The sum of num1 num2 ", {1, 7}, LexerTokenType::StringToken}},
                                        {LexerToken{"sum", {1, 31}, LexerTokenType::VarToken}},
                                        {LexerToken{")", {1, 34}, LexerTokenType::ParenClose}}

    };
    Lexer lex(actual);
    std::vector<LexerToken> m_tokens;
    for (auto token = lex.nextNWToken(); token.type != LexerTokenType::Eof; token = lex.nextNWToken())
    {
        m_tokens.emplace_back(token);
    }
    LexerEquality(m_tokens, expected);
}


TEST(curiousTest, Expression)
{
    std::string actual = "2 + 3 * 5.87 - (8 / 3)";

    std::vector<LexerToken> expected = {{LexerToken{"2", {1, 1}, LexerTokenType::IntToken}},
                                        {LexerToken{"+", {1, 3}, LexerTokenType::PlusToken}},
                                        {LexerToken{"3", {1, 5}, LexerTokenType::IntToken}},
                                        {LexerToken{"*", {1, 7}, LexerTokenType::MultiplyToken}},
                                        {LexerToken{"5.87", {1, 9}, LexerTokenType::FloatToken}},
                                        {LexerToken{"-", {1, 14}, LexerTokenType::MinusToken}},
                                        {LexerToken{"(", {1, 16}, LexerTokenType::ParenOpen}},
                                        {LexerToken{"8", {1, 17}, LexerTokenType::IntToken}},
                                        {LexerToken{"/", {1, 19}, LexerTokenType::DivideToken}},
                                        {LexerToken{"3", {1, 21}, LexerTokenType::IntToken}},
                                        {LexerToken{")", {1, 22}, LexerTokenType::ParenClose}}

    };
    Lexer lex(actual);
    std::vector<LexerToken> m_tokens;
    for (auto token = lex.nextNWToken(); token.type != LexerTokenType::Eof; token = lex.nextNWToken())
    {
        m_tokens.emplace_back(token);
    }
    LexerEquality(m_tokens, expected);

}


// Test Conditions
TEST(curiousTest, Conditions)
{
    std::string actual = "if(x ==5){ if(y >= 7)}";

    std::vector<LexerToken> expected = {{LexerToken{"if", {1, 1}, LexerTokenType::IfToken}},
                                        {LexerToken{"(", {1, 3}, LexerTokenType::ParenOpen}},
                                        {LexerToken{"x", {1, 4}, LexerTokenType::VarToken}},
                                        {LexerToken{"==", {1, 6}, LexerTokenType::EqualToken}},
                                        {LexerToken{"5", {1, 8}, LexerTokenType::IntToken}},
                                        {LexerToken{")", {1, 9}, LexerTokenType::ParenClose}},
                                        {LexerToken{"{", {1, 10}, LexerTokenType::BraceOpen}},
                                        {LexerToken{"if", {1, 12}, LexerTokenType::IfToken}},
                                        {LexerToken{"(", {1, 14}, LexerTokenType::ParenOpen}},
                                        {LexerToken{"y", {1, 15}, LexerTokenType::VarToken}},
                                        {LexerToken{">=", {1, 17}, LexerTokenType::GreaterEqualToken}},
                                        {LexerToken{"7", {1, 20}, LexerTokenType::IntToken}},
                                        {LexerToken{")", {1, 21}, LexerTokenType::ParenClose}},
                                        {LexerToken{"}", {1, 22}, LexerTokenType::BraceClose}}

    };
    Lexer lex(actual);
    std::vector<LexerToken> m_tokens;
    for (auto token = lex.nextNWToken(); token.type != LexerTokenType::Eof; token = lex.nextNWToken())
    {
        m_tokens.emplace_back(token);
    }
    LexerEquality(m_tokens, expected);

}


// Testing Files
TEST(curiousTest, Files)
{
    FileHandler filehandler;
    const char* argv[] = {"lexer", "tests\testfile_1.txt", "lkjk"};
    // ASSERT_TRUE(filehandler.ParseArguments(2, argv));

    Lexer lex(filehandler.getFileContents());

    std::vector<LexerToken> m_tokens;
    for (auto token = lex.nextNWToken(); token.type != LexerTokenType::Eof; token = lex.nextNWToken())
    {
        m_tokens.emplace_back(token);
    }
    // ASSERT_TRUE(filehandler.LexerFile(m_tokens));

}


/*TEST (curiousTest, CountTrees) {
    std::vector<LexerToken> tokens = {{LexerToken{"2", {1, 1}, LexerTokenType::IntToken}},
                                      {LexerToken{"+", {1, 3}, LexerTokenType::PlusToken}},
                                      {LexerToken{"3", {1, 5}, LexerTokenType::IntToken}},
                                      {LexerToken{"*", {1, 7}, LexerTokenType::MultiplyToken}},
                                      {LexerToken{"5.87", {1, 9}, LexerTokenType::FloatToken}},
                                      {LexerToken{"-", {1, 14}, LexerTokenType::MinusToken}},
                                      {LexerToken{"(", {1, 16}, LexerTokenType::ParenOpen}},
                                      {LexerToken{"8", {1, 17}, LexerTokenType::IntToken}},
                                      {LexerToken{"/", {1, 19}, LexerTokenType::DivideToken}},
                                      {LexerToken{"3", {1, 21}, LexerTokenType::IntToken}},
                                      {LexerToken{")", {1, 22}, LexerTokenType::ParenClose}}};

    auto ast = new Parser(tokens);

    bool x = ast->Parse();
    ASSERT_FALSE(false);

}*/