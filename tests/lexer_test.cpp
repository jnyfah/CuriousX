#include <gtest/gtest.h>
#include <string_view>
#include <vector>

#include "lexer/lexer.hpp"
#include "tools/diagnostics.hpp"

using namespace cx;

namespace
{

    struct Expected
    {
        TokenType        type;
        std::string_view value;
    };

    //! Lexes `input` to Eof (the Eof token itself is not returned).
    std::vector<Token> tokenize(std::string_view input, Diagnostics& diag)
    {
        Lexer              lexer(input, diag);
        std::vector<Token> tokens;
        for (auto t = lexer.nextToken(); t.type != TokenType::Eof; t = lexer.nextToken())
        {
            tokens.push_back(t);
        }
        return tokens;
    }

    void expectTokens(std::string_view input, const std::vector<Expected>& expected, size_t expectedErrors = 0)
    {
        Diagnostics diag;
        const auto  tokens = tokenize(input, diag);

        ASSERT_EQ(tokens.size(), expected.size()) << "token count mismatch for input: " << input;
        for (size_t i = 0; i < tokens.size(); ++i)
        {
            EXPECT_EQ(tokens[i].type, expected[i].type)
                << "type mismatch at " << i << " (got " << toString(tokens[i].type) << ")";
            EXPECT_EQ(tokens[i].value, expected[i].value) << "value mismatch at " << i;
        }
        EXPECT_EQ(diag.all().size(), expectedErrors) << "diagnostic count mismatch for input: " << input;
    }

} // namespace

// ---------------------------------------------------------------- numbers

TEST(Lexer, Integers)
{
    expectTokens("42 0 1000", {{TokenType::Int, "42"}, {TokenType::Int, "0"}, {TokenType::Int, "1000"}});
}

TEST(Lexer, Floats)
{
    expectTokens("3.14 0.5", {{TokenType::Float, "3.14"}, {TokenType::Float, "0.5"}});
}

TEST(Lexer, NegativeNumbersLexAsMinusThenNumber)
{
    // the lexer does not fold sign into the literal; that is the parser's job
    expectTokens("-17", {{TokenType::Minus, "-"}, {TokenType::Int, "17"}});
}

TEST(Lexer, TrailingDotIsAnError)
{
    expectTokens("1.", {{TokenType::Unknown, "1."}}, 1);
}

TEST(Lexer, DigitsFollowedByLettersIsAnError)
{
    expectTokens("123abc", {{TokenType::Unknown, "123abc"}}, 1);
}

TEST(Lexer, TwoDotsIsAnError)
{
    expectTokens("1.2.3", {{TokenType::Unknown, "1.2.3"}}, 1);
}

TEST(Lexer, BadNumberConsumesWholeRunAndReportsOnce)
{
    // one bad lexeme must produce exactly one diagnostic, not a cascade
    expectTokens("123abc + 1", {{TokenType::Unknown, "123abc"}, {TokenType::Plus, "+"}, {TokenType::Int, "1"}}, 1);
}

// ------------------------------------------------------------ identifiers

TEST(Lexer, Identifiers)
{
    expectTokens("x count2 _foo my_var3",
                 {{TokenType::Var, "x"},
                  {TokenType::Var, "count2"},
                  {TokenType::Var, "_foo"},
                  {TokenType::Var, "my_var3"}});
}

TEST(Lexer, Keywords)
{
    expectTokens("if else while for func return print true false",
                 {{TokenType::If, "if"},
                  {TokenType::Else, "else"},
                  {TokenType::While, "while"},
                  {TokenType::For, "for"},
                  {TokenType::Func, "func"},
                  {TokenType::Return, "return"},
                  {TokenType::Print, "print"},
                  {TokenType::Bool, "true"},
                  {TokenType::Bool, "false"}});
}

TEST(Lexer, MaximalMunchBeatsKeywords)
{
    // "iffy" must not lex as `if` + `fy`
    expectTokens("iffy elsewhere", {{TokenType::Var, "iffy"}, {TokenType::Var, "elsewhere"}});
}

TEST(Lexer, KeywordsAreCaseSensitive)
{
    expectTokens("If PRINT", {{TokenType::Var, "If"}, {TokenType::Var, "PRINT"}});
}

// --------------------------------------------------------------- operators

TEST(Lexer, SingleCharOperators)
{
    expectTokens("+ - * / % = < > !",
                 {{TokenType::Plus, "+"},
                  {TokenType::Minus, "-"},
                  {TokenType::Multiply, "*"},
                  {TokenType::Divide, "/"},
                  {TokenType::Percent, "%"},
                  {TokenType::Assign, "="},
                  {TokenType::Less, "<"},
                  {TokenType::Greater, ">"},
                  {TokenType::Not, "!"}});
}

TEST(Lexer, ComparisonOperators)
{
    expectTokens("== != <= >=",
                 {{TokenType::Equal, "=="},
                  {TokenType::NotEqual, "!="},
                  {TokenType::LessEqual, "<="},
                  {TokenType::GreaterEqual, ">="}});
}

TEST(Lexer, CompoundAssignment)
{
    expectTokens("+= -= *= /=",
                 {{TokenType::PlusEq, "+="},
                  {TokenType::MinusEq, "-="},
                  {TokenType::MultiplyEq, "*="},
                  {TokenType::DivideEq, "/="}});
}

TEST(Lexer, OperatorsWithoutSurroundingSpace)
{
    expectTokens("a+b<=c",
                 {{TokenType::Var, "a"},
                  {TokenType::Plus, "+"},
                  {TokenType::Var, "b"},
                  {TokenType::LessEqual, "<="},
                  {TokenType::Var, "c"}});
}

TEST(Lexer, Punctuation)
{
    expectTokens("( ) { } ,",
                 {{TokenType::ParenOpen, "("},
                  {TokenType::ParenClose, ")"},
                  {TokenType::BracesOpen, "{"},
                  {TokenType::BracesClose, "}"},
                  {TokenType::Comma, ","}});
}

// ----------------------------------------------------------------- strings

TEST(Lexer, StringLiteralExcludesQuotes)
{
    expectTokens(R"("hello")", {{TokenType::String, "hello"}});
}

TEST(Lexer, StringLiteralKeepsInnerSpacesAndPunctuation)
{
    expectTokens(R"("hello, world 42!")", {{TokenType::String, "hello, world 42!"}});
}

TEST(Lexer, EmptyStringLiteral)
{
    expectTokens(R"("")", {{TokenType::String, ""}});
}

TEST(Lexer, UnterminatedStringAtEof)
{
    expectTokens("\"abc", {{TokenType::Unknown, "abc"}}, 1);
}

TEST(Lexer, UnterminatedStringStopsAtNewline)
{
    // the newline must survive: it is the statement terminator
    expectTokens("\"abc\nx",
                 {{TokenType::Unknown, "abc"}, {TokenType::Newline, "\n"}, {TokenType::Var, "x"}},
                 1);
}

// ---------------------------------------------------------------- comments

TEST(Lexer, CommentRunsToEndOfLine)
{
    expectTokens("# a comment", {{TokenType::Comment, "# a comment"}});
}

TEST(Lexer, CommentStopsAtNewline)
{
    expectTokens("x # note\ny",
                 {{TokenType::Var, "x"},
                  {TokenType::Comment, "# note"},
                  {TokenType::Newline, "\n"},
                  {TokenType::Var, "y"}});
}

// -------------------------------------------------------------- whitespace

TEST(Lexer, SpacesAndTabsAreSkippedButNewlinesAreNot)
{
    expectTokens("a \t b\nc",
                 {{TokenType::Var, "a"}, {TokenType::Var, "b"}, {TokenType::Newline, "\n"}, {TokenType::Var, "c"}});
}

TEST(Lexer, EmptyInputProducesNoTokens)
{
    expectTokens("", {});
}

TEST(Lexer, EofRepeatsForever)
{
    Diagnostics diag;
    Lexer       lexer("x", diag);

    EXPECT_EQ(lexer.nextToken().type, TokenType::Var);
    for (int i = 0; i < 5; ++i)
    {
        EXPECT_EQ(lexer.nextToken().type, TokenType::Eof);
    }
}

// --------------------------------------------------------------- locations

TEST(Lexer, TracksColumns)
{
    Diagnostics diag;
    const auto  tokens = tokenize("x = 42", diag);

    ASSERT_EQ(tokens.size(), 3u);
    EXPECT_EQ(tokens[0].location.getLine(), 1);
    EXPECT_EQ(tokens[0].location.getCol(), 1);
    EXPECT_EQ(tokens[1].location.getCol(), 3);
    EXPECT_EQ(tokens[2].location.getCol(), 5);
}

TEST(Lexer, TracksLines)
{
    Diagnostics diag;
    const auto  tokens = tokenize("a\nb\nc", diag);

    ASSERT_EQ(tokens.size(), 5u);
    EXPECT_EQ(tokens[0].location.getLine(), 1);   // a
    EXPECT_EQ(tokens[2].location.getLine(), 2);   // b
    EXPECT_EQ(tokens[4].location.getLine(), 3);   // c
    EXPECT_EQ(tokens[4].location.getCol(), 1);
}

// ------------------------------------------------------------- diagnostics

TEST(Lexer, UnknownCharacterIsReportedOnce)
{
    expectTokens("a $ b", {{TokenType::Var, "a"}, {TokenType::Unknown, "$"}, {TokenType::Var, "b"}}, 1);
}

TEST(Lexer, ValidProgramProducesNoDiagnostics)
{
    Diagnostics diag;
    tokenize("x = 42\nif (x >= 0) {\n  print(\"hi\")\n}\n", diag);
    EXPECT_FALSE(diag.hasErrors());
}

TEST(Lexer, DiagnosticsAreReportedToTheCallersEngine)
{
    // guards against Diagnostics being stored by value in the Lexer
    Diagnostics diag;
    tokenize("1.", diag);
    ASSERT_TRUE(diag.hasErrors());
    EXPECT_EQ(diag.all().front().loc.getLine(), 1);
}
