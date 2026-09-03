#include <gtest/gtest.h>
#include <string>
#include <string_view>

#include "parser/arena.hpp"
#include "parser/parser.hpp"

using namespace cx;

namespace
{

    //! Renders the AST as an s-expression so tests can assert on shape in one line.
    //! `(+ 1 (* 2 3))` says everything a hand-written pointer walk would, and says it readably.
    std::string dump(const Node* n)
    {
        if (n == nullptr)
        {
            return "<null>";
        }

        const std::string value(n->token.value);

        switch (n->kind)
        {
            case NodeKind::Number:
            case NodeKind::String:
            case NodeKind::Bool:
            case NodeKind::Identifier:
                return value;

            case NodeKind::Error:
                return "<error>";

            case NodeKind::BinaryExpr:
            {
                const auto* b = static_cast<const BinaryNode*>(n);
                return "(" + value + " " + dump(b->left) + " " + dump(b->right) + ")";
            }

            case NodeKind::UnaryExpr:
            {
                const auto* u = static_cast<const UnaryNode*>(n);
                return "(" + value + " " + dump(u->operand) + ")";
            }

            case NodeKind::Print:
            {
                const auto* p = static_cast<const PrintNode*>(n);
                return "(print " + dump(p->expression) + ")";
            }

            case NodeKind::ReturnStmt:
            {
                const auto* r = static_cast<const ReturnNode*>(n);
                return r->expression ? "(return " + dump(r->expression) + ")" : "(return)";
            }

            case NodeKind::IfStmt:
            {
                const auto* i   = static_cast<const IfNode*>(n);
                std::string out = "(if " + dump(i->condition) + " [";
                for (const auto* s : i->then) out += dump(s) + " ";
                out += "]";
                if (!i->nelse.empty())
                {
                    out += " [";
                    for (const auto* s : i->nelse) out += dump(s) + " ";
                    out += "]";
                }
                return out + ")";
            }

            case NodeKind::WhileStmt:
            {
                const auto* w   = static_cast<const WhileNode*>(n);
                std::string out = "(while " + dump(w->condition) + " [";
                for (const auto* s : w->loop) out += dump(s) + " ";
                return out + "])";
            }

            case NodeKind::FuncDecl:
            {
                const auto* f   = static_cast<const FuncNode*>(n);
                std::string out = "(func " + dump(f->name) + " (";
                for (size_t i = 0; i < f->parameters.size(); ++i)
                {
                    out += (i ? " " : "") + dump(f->parameters[i]);
                }
                out += ") [";
                for (const auto* s : f->body) out += dump(s) + " ";
                return out + "])";
            }

            case NodeKind::FuncCall:
            {
                const auto* c   = static_cast<const CallNode*>(n);
                std::string out = "(call " + dump(c->callee);
                for (const auto* a : c->arguments) out += " " + dump(a);
                return out + ")";
            }

            default:
                return "<" + value + ">";
        }
    }

    struct Parsed
    {
        Diagnostics  diag;
        ProgramNode* root = nullptr;

        //! Every statement rendered, space-separated.
        std::string  text() const
        {
            std::string out;
            for (size_t i = 0; i < root->statements.size(); ++i)
            {
                out += (i ? " " : "") + dump(root->statements[i]);
            }
            return out;
        }

        size_t errors() const { return diag.all().size(); }
        size_t statements() const { return root->statements.size(); }
    };

    //! Parses `src`. The Parsed object owns the diagnostics; the arena lives inside
    //! the Parser, so the tree must be rendered before this returns.
    std::string parseToString(std::string_view src, Diagnostics& diag)
    {
        Arena arena(4096);
        Lexer  lexer(src, diag);
        Parser parser(lexer, diag, arena);

        ProgramNode* root = parser.Parse();

        std::string out;
        for (size_t i = 0; i < root->statements.size(); ++i)
        {
            out += (i ? " " : "") + dump(root->statements[i]);
        }
        return out;
    }

    //! Asserts the whole program renders to `expected` with no diagnostics.
    void expectAst(std::string_view src, std::string_view expected)
    {
        Diagnostics diag;
        const auto  got = parseToString(src, diag);

        EXPECT_EQ(got, expected) << "for input: " << src;
        EXPECT_EQ(diag.all().size(), 0u) << "unexpected diagnostics for: " << src;
        for (const auto& d : diag.all())
        {
            ADD_FAILURE() << "  " << d.loc.toString() << " " << d.message;
        }
    }

    //! Asserts `src` produces exactly `expectedErrors` diagnostics.
    void expectErrors(std::string_view src, size_t expectedErrors)
    {
        Diagnostics diag;
        parseToString(src, diag);
        EXPECT_EQ(diag.all().size(), expectedErrors) << "for input: " << src;
    }

} // namespace

// ------------------------------------------------------------- precedence

TEST(Parser, MultiplyBindsTighterThanAdd)
{
    expectAst("x = 1 + 2 * 3;", "(= x (+ 1 (* 2 3)))");
    expectAst("x = 1 * 2 + 3;", "(= x (+ (* 1 2) 3))");
}

TEST(Parser, ParenthesesOverridePrecedence)
{
    expectAst("x = (1 + 2) * 3;", "(= x (* (+ 1 2) 3))");
}

TEST(Parser, ArithmeticIsLeftAssociative)
{
    expectAst("x = 1 - 2 - 3;", "(= x (- (- 1 2) 3))");
    expectAst("x = 8 / 4 / 2;", "(= x (/ (/ 8 4) 2))");
}

TEST(Parser, ComparisonBindsLooserThanArithmetic)
{
    expectAst("x = 1 + 2 < 4;", "(= x (< (+ 1 2) 4))");
}

TEST(Parser, EqualityBindsLooserThanComparison)
{
    expectAst("x = a < b == c;", "(= x (== (< a b) c))");
}

TEST(Parser, AndBindsTighterThanOr)
{
    expectAst("x = a | b & c;", "(= x (| a (& b c)))");
}

TEST(Parser, ModuloBindsLikeMultiply)
{
    expectAst("x = 1 + 6 % 4;", "(= x (+ 1 (% 6 4)))");
}

// ------------------------------------------------------------------ unary

TEST(Parser, UnaryBindsTighterThanMultiply)
{
    expectAst("x = -a * b;", "(= x (* (- a) b))");
}

TEST(Parser, UnaryNot)
{
    expectAst("x = !a == b;", "(= x (== (! a) b))");
}

TEST(Parser, UnaryIsRightAssociative)
{
    expectAst("x = --a;", "(= x (- (- a)))");
}

// ------------------------------------------------------------- assignment

TEST(Parser, AssignmentIsRightAssociative)
{
    expectAst("a = b = 1;", "(= a (= b 1))");
}

TEST(Parser, AssignmentToNonVariableIsAnError)
{
    expectErrors("1 = 2;", 1);
}

TEST(Parser, AssignmentToCallIsAnError)
{
    expectErrors("f(x) = 2;", 1);
}

// ------------------------------------------------------------- statements

TEST(Parser, MultipleStatements)
{
    expectAst("x = 1;\ny = 2;", "(= x 1) (= y 2)");
}

TEST(Parser, CommentsAndBlankLinesAreInvisible)
{
    expectAst("# leading\nx = 1;  # trailing\n\ny = 2;", "(= x 1) (= y 2)");
}

TEST(Parser, PrintStatement)
{
    expectAst("print x + 1;", "(print (+ x 1))");
}

TEST(Parser, ReturnWithValue)
{
    expectAst("func f() { return 1 + 2; }", "(func f () [(return (+ 1 2)) ])");
}

TEST(Parser, ReturnWithoutValue)
{
    expectAst("func f() { return; }", "(func f () [(return) ])");
}

// -------------------------------------------------------------------- if

TEST(Parser, IfWithoutElse)
{
    expectAst("if (x > 1) { y = 2; }", "(if (> x 1) [(= y 2) ])");
}

TEST(Parser, IfElse)
{
    expectAst("if (x) { y = 1; } else { y = 2; }", "(if x [(= y 1) ] [(= y 2) ])");
}

TEST(Parser, ElseIfChain)
{
    expectAst("if (a) { x = 1; } else if (b) { x = 2; } else { x = 3; }",
              "(if a [(= x 1) ] [(if b [(= x 2) ] [(= x 3) ]) ])");
}

TEST(Parser, EmptyBlock)
{
    expectAst("if (x) { }", "(if x [])");
}

TEST(Parser, NestedIf)
{
    expectAst("if (a) { if (b) { x = 1; } }", "(if a [(if b [(= x 1) ]) ])");
}

// ----------------------------------------------------------------- while

TEST(Parser, WhileLoop)
{
    expectAst("while (x < 10) { x = x + 1; }", "(while (< x 10) [(= x (+ x 1)) ])");
}

// ------------------------------------------------------------- functions

TEST(Parser, FuncNoParameters)
{
    expectAst("func f() { x = 1; }", "(func f () [(= x 1) ])");
}

TEST(Parser, FuncOneParameter)
{
    expectAst("func f(a) { return a; }", "(func f (a) [(return a) ])");
}

TEST(Parser, FuncManyParameters)
{
    expectAst("func add(a, b, c) { return a + b + c; }",
              "(func add (a b c) [(return (+ (+ a b) c)) ])");
}

TEST(Parser, FuncNameIsNotTheKeyword)
{
    // regression: parseFuncDecl used to store the 'func' token as the name
    Diagnostics diag;
    EXPECT_NE(parseToString("func add() { }", diag).find("add"), std::string::npos);
}

// ----------------------------------------------------------------- calls

TEST(Parser, CallNoArguments)
{
    expectAst("x = f();", "(= x (call f))");
}

TEST(Parser, CallManyArguments)
{
    expectAst("x = add(1, 2);", "(= x (call add 1 2))");
}

TEST(Parser, CallArgumentsAreFullExpressions)
{
    expectAst("x = f(a + 1, -b);", "(= x (call f (+ a 1) (- b)))");
}

TEST(Parser, NestedCalls)
{
    expectAst("x = f(g(1));", "(= x (call f (call g 1)))");
}

TEST(Parser, CallAsStatement)
{
    expectAst("f(1);", "(call f 1)");
}

TEST(Parser, IdentifierWithoutParensIsNotACall)
{
    expectAst("x = f;", "(= x f)");
}

// ------------------------------------------------------ errors & recovery

TEST(Parser, MissingSemicolon)
{
    expectErrors("x = 1", 1);
}

TEST(Parser, MissingClosingBrace)
{
    expectErrors("if (x) { y = 1;", 1);
}

TEST(Parser, MissingClosingParen)
{
    expectErrors("x = (1 + 2;", 1);
}

TEST(Parser, UnmatchedClosingBraceIsOneError)
{
    expectErrors("}", 1);
}

TEST(Parser, ExpectedExpression)
{
    expectErrors("x = ;", 1);
}

TEST(Parser, BadParameterIsReported)
{
    expectErrors("func f(1) { }", 1);
}

TEST(Parser, ParsingContinuesAfterAnError)
{
    // the broken statement must not swallow the ones after it
    Diagnostics diag;
     Arena arena(4096);
    Lexer       lexer("x = ;\ny = 2;\nz = 3;", diag);
    Parser      parser(lexer, diag, arena);
    auto*       root = parser.Parse();

    EXPECT_TRUE(diag.hasErrors());
    EXPECT_GE(root->statements.size(), 3u) << "recovery lost the later statements";
    EXPECT_EQ(dump(root->statements.back()), "(= z 3)");
}

TEST(Parser, TerminatesOnTruncatedInput)
{
    // each of these used to be able to spin forever
    expectErrors("func f(", 2);
    for (std::string_view src : {"if (", "while (", "func f(a,", "x = (", "if (x) {"})
    {
        Diagnostics diag;
        parseToString(src, diag);
        EXPECT_TRUE(diag.hasErrors()) << "for input: " << src;
    }
}

TEST(Parser, DeepNestingIsRejectedNotCrashed)
{
    const std::string src = "x = " + std::string(500, '(') + "1" + std::string(500, ')') + ";";
    Diagnostics       diag;
    parseToString(src, diag);
    EXPECT_TRUE(diag.hasErrors());
}

TEST(Parser, ValidProgramProducesNoDiagnostics)
{
    Diagnostics diag;
    parseToString("func add(a, b) {\n"
                  "  return a + b;\n"
                  "}\n"
                  "x = add(1, 2);\n"
                  "if (x > 2) {\n"
                  "  print x;\n"
                  "} else {\n"
                  "  print 0;\n"
                  "}\n"
                  "while (x < 10) {\n"
                  "  x = x + 1;\n"
                  "}\n",
                  diag);
    EXPECT_FALSE(diag.hasErrors());
}
