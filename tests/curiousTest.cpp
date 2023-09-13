#include "gtest/gtest.h"

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
        while (std::getline(expected, lineB)) { EXPECT_EQ(lineA, lineB); }
    }
}


TEST(curiousTest, VariableAssign)
{
    std::string actual = "num1 = 1.5";

    std::vector<LexerToken> expected = { { LexerToken{ "num1", { 1, 1 }, LexerTokenType::VarToken } },
        { LexerToken{ "=", { 1, 6 }, LexerTokenType::AssignToken } },
        { LexerToken{ "1.5", { 1, 8 }, LexerTokenType::FloatToken } }

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
    std::string actual = "print(x  + y)";

    std::vector<LexerToken> expected = { { LexerToken{ "print", { 1, 1 }, LexerTokenType::PrintToken } },
        { LexerToken{ "(", { 1, 6 }, LexerTokenType::ParenOpen } },
        { LexerToken{ "x", { 1, 7 }, LexerTokenType::VarToken } },
        { LexerToken{ "+", { 1, 10 }, LexerTokenType::PlusToken } },
        { LexerToken{ "y", { 1, 12 }, LexerTokenType::VarToken } },
        { LexerToken{ ")", { 1, 13 }, LexerTokenType::ParenClose } }

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

    std::vector<LexerToken> expected = { { LexerToken{ "2", { 1, 1 }, LexerTokenType::IntToken } },
        { LexerToken{ "+", { 1, 3 }, LexerTokenType::PlusToken } },
        { LexerToken{ "3", { 1, 5 }, LexerTokenType::IntToken } },
        { LexerToken{ "*", { 1, 7 }, LexerTokenType::MultiplyToken } },
        { LexerToken{ "5.87", { 1, 9 }, LexerTokenType::FloatToken } },
        { LexerToken{ "-", { 1, 14 }, LexerTokenType::MinusToken } },
        { LexerToken{ "(", { 1, 16 }, LexerTokenType::ParenOpen } },
        { LexerToken{ "8", { 1, 17 }, LexerTokenType::IntToken } },
        { LexerToken{ "/", { 1, 19 }, LexerTokenType::DivideToken } },
        { LexerToken{ "3", { 1, 21 }, LexerTokenType::IntToken } },
        { LexerToken{ ")", { 1, 22 }, LexerTokenType::ParenClose } }

    };
    Lexer lex(actual);
    std::vector<LexerToken> m_tokens;
    for (auto token = lex.nextNWToken(); token.type != LexerTokenType::Eof; token = lex.nextNWToken())
    {
        m_tokens.emplace_back(token);
    }
    LexerEquality(m_tokens, expected);
}
