#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <string_view>
#include "interp/interp.hpp"
#include "parser/arena.hpp"
#include "parser/parser.hpp"
#include "sema/sema.hpp"

using namespace cx;

namespace
{
    struct Run
    {
        Diagnostics diag;
        Arena       arena{1 << 16};
        Sema        sema{diag};
        std::string out;

        bool        operator()(std::string_view src)
        {
            Lexer        lexer(src, diag);
            Parser       parser(lexer, diag, arena);

            ProgramNode* root = parser.Parse();
            sema.analyze(root);

            if (diag.hasErrors())
            {
                return false;
            }

            Interpreter interp(sema.table());
            interp.run(*root);
            out = interp.output();
            return true;
        }

        std::string messages() const
        {
            std::string m;
            for (const auto& d : diag.all())
                m += d.message + "\n";
            return m;
        }
    };

    void expectOutput(std::string_view src, std::string_view expected)
    {
        Run run;
        ASSERT_TRUE(run(src)) << "did not compile: " << src << "\n" << run.messages();
        EXPECT_EQ(run.out, expected) << "for: " << src;
    }

} // namespace

// ------------------------------------------------------------- literals

TEST(Interp, PrintsEachType)
{
    expectOutput("print 42;", "42\n");
    expectOutput("print 2.5;", "2.5\n");
    expectOutput("print true;", "true\n");
    expectOutput("print false;", "false\n");
    expectOutput("print \"hi\";", "hi\n");
}

TEST(Interp, WholeFloatsKeepADecimalPoint)
{
    expectOutput("print 3.0;", "3.0\n");
}

TEST(Interp, StringsKeepTheirSpacing)
{
    expectOutput("print \"a b  c\";", "a b  c\n");
}

// ----------------------------------------------------------- arithmetic

TEST(Interp, PrecedenceIsRespected)
{
    expectOutput("print 1 + 2 * 3;", "7\n");
    expectOutput("print (1 + 2) * 3;", "9\n");
}

TEST(Interp, SubtractionIsLeftAssociative)
{
    // right-associativity would give 10 - (3 - 2) == 9
    expectOutput("print 10 - 3 - 2;", "5\n");
    expectOutput("print 16 / 4 / 2;", "2\n");
}

TEST(Interp, IntegerDivisionTruncates)
{
    expectOutput("print 7 / 2;", "3\n");
    expectOutput("print 7 % 2;", "1\n");
}

TEST(Interp, FloatArithmetic)
{
    expectOutput("print 1.5 + 2.25;", "3.75\n");
    expectOutput("print 7.5 / 2.5;", "3.0\n");
}

TEST(Interp, IntegerOverflowWrapsLikeTheHardware)
{
    // 32-bit wraparound, matching RV32 -- not 64-bit, and not undefined behaviour
    expectOutput("print 2147483647 + 1;", "-2147483648\n");
    expectOutput("print 100000 * 100000;", "1410065408\n");
}

TEST(Interp, UnaryOperators)
{
    expectOutput("x = 5; print -x;", "-5\n");
    expectOutput("print !true;", "false\n");
    expectOutput("x = 5; print !(x > 10);", "true\n");
}

// ---------------------------------------------------------- comparisons

TEST(Interp, Comparisons)
{
    expectOutput("print 1 < 2;", "true\n");
    expectOutput("print 2 <= 2;", "true\n");
    expectOutput("print 3 > 4;", "false\n");
    expectOutput("print 3 == 3;", "true\n");
    expectOutput("print 3 != 3;", "false\n");
}

TEST(Interp, LogicalOperators)
{
    expectOutput("print true & false;", "false\n");
    expectOutput("print true | false;", "true\n");
}

TEST(Interp, StringEquality)
{
    expectOutput("print \"a\" == \"a\";", "true\n");
    expectOutput("print \"a\" == \"b\";", "false\n");
}

// ----------------------------------------------------------- statements

TEST(Interp, StatementsRunInOrder)
{
    expectOutput("print 1; print 2; print 3;", "1\n2\n3\n");
}

TEST(Interp, VariablesHoldTheirValue)
{
    expectOutput("x = 1; y = x + 1; x = 10; print x; print y;", "10\n2\n");
}

TEST(Interp, IfElse)
{
    expectOutput("x = 3; if (x > 2) { print \"big\"; } else { print \"small\"; }", "big\n");
    expectOutput("x = 1; if (x > 2) { print \"big\"; } else { print \"small\"; }", "small\n");
}

TEST(Interp, IfWithoutElseRunsNothing)
{
    expectOutput("x = 1; if (x > 2) { print \"no\"; } print \"after\";", "after\n");
}

TEST(Interp, ElseIfChain)
{
    const std::string_view src = "if (x > 0) { print 1; } else if (x == 0) { print 2; } else { print 3; }";
    expectOutput(std::string("x = 5; ").append(src), "1\n");
    expectOutput(std::string("x = 0; ").append(src), "2\n");
    expectOutput(std::string("x = 0 - 5; ").append(src), "3\n");
}

TEST(Interp, NestedIf)
{
    expectOutput("a = 1; b = 2; if (a > 0) { if (b > 1) { print \"both\"; } }", "both\n");
}

TEST(Interp, WhileLoop)
{
    expectOutput("i = 0; while (i < 3) { print i; i = i + 1; }", "0\n1\n2\n");
}

TEST(Interp, WhileWithAFalseConditionNeverRuns)
{
    expectOutput("i = 5; while (i < 3) { print i; } print \"done\";", "done\n");
}

TEST(Interp, NestedLoops)
{
    expectOutput("i = 0; while (i < 2) { j = 0; while (j < 2) { print i * 10 + j; j = j + 1; } i = i + 1; }", "0\n1\n10\n11\n");
}

// ------------------------------------------------------------ functions

TEST(Interp, CallWithArguments)
{
    expectOutput("func add(a, b) { return a + b; } print add(3, 4);", "7\n");
}

TEST(Interp, CallWithNoArguments)
{
    expectOutput("func answer() { return 42; } print answer();", "42\n");
}

TEST(Interp, ArgumentsAreEvaluatedInTheCallersFrame)
{
    expectOutput("x = 10; y = 5; func f(p, q) { return p - q; } print f(x + 1, y);", "6\n");
}

TEST(Interp, ArgumentsArePassedByValue)
{
    expectOutput("func f(p) { p = 99; return p; } x = 1; print f(x); print x;", "99\n1\n");
}

TEST(Interp, EarlyReturnSkipsTheRest)
{
    const std::string_view src = "func f(n) { if (n < 0) { return 1; } if (n == 0) { return 2; } return 3; }";
    expectOutput(std::string(src).append(" print f(0 - 1);"), "1\n");
    expectOutput(std::string(src).append(" print f(0);"), "2\n");
    expectOutput(std::string(src).append(" print f(5);"), "3\n");
}

TEST(Interp, ReturnUnwindsOutOfNestedBlocks)
{
    expectOutput("func f(n) { while (n > 0) { if (n == 2) { return n; } n = n - 1; } return 0; } print f(5);", "2\n");
}

TEST(Interp, VoidFunctionRunsForItsEffect)
{
    expectOutput("func g(n) { print n; } g(7);", "7\n");
}

TEST(Interp, Recursion)
{
    expectOutput("func fact(n) { if (n < 2) { return 1; } return n * fact(n - 1); } print fact(5);", "120\n");
}

TEST(Interp, DeepRecursionKeepsFramesSeparate)
{
    expectOutput("func down(n) { if (n == 0) { return 0; } print n; return down(n - 1); } down(4);", "4\n3\n2\n1\n");
}

TEST(Interp, MutualRecursion)
{
    expectOutput(
        "func isEven(n) { if (n == 0) { return true; } return isOdd(n - 1); } "
        "func isOdd(n) { if (n == 0) { return false; } return isEven(n - 1); } "
        "print isEven(4);",
        "true\n");
}

TEST(Interp, FunctionCalledSeveralTimes)
{
    expectOutput("func twice(n) { return n * 2; } print twice(1); print twice(2); print twice(3);", "2\n4\n6\n");
}

TEST(Interp, CallInsideAnExpression)
{
    expectOutput("func inc(n) { return n + 1; } print inc(1) + inc(2);", "5\n");
}

TEST(Interp, NestedCalls)
{
    expectOutput("func inc(n) { return n + 1; } print inc(inc(inc(0)));", "3\n");
}

TEST(Interp, LocalsInAFunctionDoNotDisturbTheCaller)
{
    expectOutput("x = 1; func f(n) { y = n * 10; return y; } print f(2); print x;", "20\n1\n");
}

// --------------------------------------------------------------- blocks

TEST(Interp, BlockLocalsGetTheirOwnSlots)
{
    expectOutput("x = 1; if (x > 0) { y = 2; print y; } print x;", "2\n1\n");
}

TEST(Interp, LoopBodyLocalIsReassignedEachIteration)
{
    expectOutput("i = 0; while (i < 3) { t = i * 2; print t; i = i + 1; }", "0\n2\n4\n");
}
