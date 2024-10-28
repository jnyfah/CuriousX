#include "Compiler.hpp"
#include <gtest/gtest.h>

class CompilerIntegrationTest : public ::testing::Test
{
  protected:
    CompilerOutput output;

    bool compile(const std::string& source)
    {
        Compiler compiler(source, output);
        return compiler.compile();
    }
};

TEST_F(CompilerIntegrationTest, SimpleProgram)
{
    EXPECT_FALSE(compile(R"(
x = 42
if x > 0:
    y = x + 10
    print(y)
)"));
}

TEST_F(CompilerIntegrationTest, ComplexExpression)
{
    EXPECT_TRUE(compile(R"(
result = 10 * (20 + 30) / 2
)"));
}
