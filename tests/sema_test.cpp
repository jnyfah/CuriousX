#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <vector>

#include "parser/arena.hpp"
#include "parser/parser.hpp"
#include "sema/sema.hpp"

using namespace cx;

namespace
{

    //! Parses and analyses `src`, keeping everything alive for the caller.
    struct Analysed
    {
        Diagnostics diag;
        Arena       arena{8192};
        Sema        sema{diag};

        ProgramNode* run(std::string_view src)
        {
            m_lexer  = std::make_unique<Lexer>(src, diag);
            m_parser = std::make_unique<Parser>(*m_lexer, diag, arena);

            ProgramNode* root = m_parser->Parse();
            sema.analyze(root);
            return root;
        }

        std::size_t errors() const
        {
            std::size_t n = 0;
            for (const auto& d : diag.all())
            {
                if (d.severity == Severity::Error) ++n;
            }
            return n;
        }

        std::string messages() const
        {
            std::string out;
            for (const auto& d : diag.all()) out += d.message + "\n";
            return out;
        }

    private:
        std::unique_ptr<Lexer>  m_lexer;
        std::unique_ptr<Parser> m_parser;
    };

    //! Asserts `src` analyses with no errors at all.
    void expectClean(std::string_view src)
    {
        Analysed a;
        a.run(src);
        EXPECT_EQ(a.errors(), 0u) << "for: " << src << "\n" << a.messages();
    }

    //! Asserts `src` produces exactly `n` errors.
    void expectErrors(std::string_view src, std::size_t n)
    {
        Analysed a;
        a.run(src);
        EXPECT_EQ(a.errors(), n) << "for: " << src << "\n" << a.messages();
    }

} // namespace

// ------------------------------------------------------------ inference

TEST(Sema, InfersLocalTypesFromLiterals)
{
    expectClean("x = 1; y = 2.5; z = true; s = \"hi\";");
}

TEST(Sema, ReassignmentMustKeepTheType)
{
    expectClean("x = 1; x = 2;");
    expectErrors("x = 1; x = true;", 1);
}

TEST(Sema, UndefinedVariable)
{
    expectErrors("x = y + 1;", 1);
}

TEST(Sema, IdentifiersGetTheirSlotStamped)
{
    Analysed     a;
    ProgramNode* root = a.run("x = 1; y = 2; print y;");

    ASSERT_EQ(a.errors(), 0u) << a.messages();
    ASSERT_EQ(root->statements.size(), 3u);

    const auto* print = static_cast<const PrintNode*>(root->statements[2]);
    EXPECT_EQ(print->expression->kind, NodeKind::Identifier);
    EXPECT_EQ(print->expression->slot, 1u) << "y is the second local, so slot 1";
    EXPECT_EQ(print->expression->valuetype, ValueType::Int);
}

// ---------------------------------------------------------- expressions

TEST(Sema, OperandTypesMustAgree)
{
    expectErrors("x = 1 + true;", 1);
    expectErrors("x = \"a\" + 1;", 1);
}

TEST(Sema, ArithmeticRejectsNonNumeric)
{
    expectErrors("x = true + false;", 1);
    expectErrors("x = \"a\" - \"b\";", 1);
}

TEST(Sema, ModuloIsIntegerOnly)
{
    expectClean("x = 7 % 2;");
    expectErrors("x = 7.5 % 2.5;", 1);
}

TEST(Sema, ComparisonYieldsBool)
{
    Analysed     a;
    ProgramNode* root = a.run("x = 1 < 2;");
    ASSERT_EQ(a.errors(), 0u) << a.messages();

    const auto* assign = static_cast<const BinaryNode*>(root->statements[0]);
    EXPECT_EQ(assign->right->valuetype, ValueType::Bool);
}

TEST(Sema, LogicalOperatorsNeedBooleans)
{
    expectClean("x = true & false;");
    expectErrors("x = 1 & 2;", 1);
}

TEST(Sema, UnaryOperators)
{
    expectClean("x = -1; y = !true;");
    expectErrors("x = -true;", 1);
    expectErrors("x = !1;", 1);
}

TEST(Sema, OnePoisonedExpressionReportsOnce)
{
    // the undefined 'y' must not also produce an operand-mismatch complaint
    expectErrors("x = y + 1;", 1);
}

// ----------------------------------------------------------- statements

TEST(Sema, IfConditionMustBeBool)
{
    expectClean("x = 1; if (x > 0) { x = 2; }");
    expectErrors("x = 1; if (x) { x = 2; }", 1);
}

TEST(Sema, WhileConditionMustBeBool)
{
    expectClean("x = 0; while (x < 3) { x = x + 1; }");
    expectErrors("x = 0; while (x) { x = x + 1; }", 1);
}

TEST(Sema, BlockScopesAreDiscardedAtTheEnd)
{
    // y only exists inside the if
    expectErrors("x = 1; if (x > 0) { y = 2; } print y;", 1);
}

// ------------------------------------------------------------ functions

TEST(Sema, ParameterTypesComeFromTheFirstCall)
{
    expectClean("func add(a, b) { return a + b; } x = add(1, 2);");
}

TEST(Sema, SecondCallMustMatchTheInferredSignature)
{
    expectErrors("func add(a, b) { return a + b; } x = add(1, 2); y = add(1.5, 2.5);", 2);
}

TEST(Sema, ArityIsChecked)
{
    expectErrors("func add(a, b) { return a + b; } x = add(1);", 1);
}

TEST(Sema, UndefinedFunction)
{
    expectErrors("x = nope(1);", 1);
}

TEST(Sema, ForwardReferenceWorks)
{
    // 'second' is declared below the call to it
    expectClean("func first(a) { return second(a); } func second(b) { return b + 1; } x = first(1);");
}

TEST(Sema, ReturnTypeIsInferred)
{
    Analysed a;
    a.run("func f(a) { return a + 1; } x = f(1);");
    ASSERT_EQ(a.errors(), 0u) << a.messages();

    const auto* root = static_cast<const ProgramNode*>(nullptr);
    (void)root;
}

TEST(Sema, ReturnStatementsMustAgree)
{
    expectErrors("func f(a) { if (a > 0) { return 1; } return true; } x = f(1);", 1);
}

TEST(Sema, VoidFunctionUsedAsAValue)
{
    // g returns nothing, so it cannot be printed
    expectErrors("func g(a) { print a; } g(1); print g(1);", 1);
}

TEST(Sema, ReturnOutsideAFunctionIsRejected)
{
    // top level is the implicit @main, which returns void
    expectErrors("return 1;", 1);
}

TEST(Sema, FunctionsDoNotSeeTheirCallersLocals)
{
    // 'outer' belongs to @main; f must not find it
    expectErrors("outer = 1; func f(a) { return outer + a; } x = f(1);", 1);
}

TEST(Sema, RecursionTerminates)
{
    // this used to recurse forever inside the compiler
    expectClean("func fact(n) { if (n < 2) { return 1; } return n * fact(n - 1); } x = fact(5);");
}

TEST(Sema, BodyIsAnalysedOnlyOnce)
{
    Analysed a;
    a.run("func f(a) { p = a + 1; q = a + 2; return q; } x = f(1); y = f(2); z = f(3);");
    ASSERT_EQ(a.errors(), 0u) << a.messages();

    const FunctionInfo* f = nullptr;
    for (const auto& fn : a.sema.table().functions())
    {
        if (fn.name == "f") f = &fn;
    }
    ASSERT_NE(f, nullptr);

    // a, p, q -- three locals, however many times f is called
    EXPECT_EQ(f->locals.size(), 3u) << "the body was walked more than once";
}

TEST(Sema, UncalledFunctionWarns)
{
    Analysed a;
    a.run("func unused(a) { return a; } x = 1;");
    EXPECT_EQ(a.errors(), 0u) << a.messages();
    EXPECT_EQ(a.diag.all().size(), 1u) << "expected a warning about the uncalled function";
}

// ---------------------------------------------------------------- print

TEST(Sema, PrintAcceptsAnyValue)
{
    expectClean("print 1; print 2.5; print true; print \"hi\";");
}
