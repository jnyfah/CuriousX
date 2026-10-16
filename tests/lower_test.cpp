#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "lower/lower.hpp"
#include "parser/arena.hpp"
#include "parser/parser.hpp"
#include "sema/sema.hpp"

using namespace cx;

namespace
{

    //! Lexes, parses, analyses and lowers `src`, keeping everything alive for the caller.
    struct Lowered
    {
        Diagnostics               diag;
        Arena                     arena{1 << 16};
        Sema                      sema{diag};
        std::span<Instruction>    rows;
        std::span<FunctionLayout> layouts;

        bool                      run(std::string_view src)
        {
            m_lexer           = std::make_unique<Lexer>(src, diag);
            m_parser          = std::make_unique<Parser>(*m_lexer, diag, arena);

            ProgramNode* root = m_parser->Parse();
            sema.analyze(root);

            if (diag.hasErrors())
            {
                return false;
            }

            m_lower = std::make_unique<Lower>(arena, sema.table());
            rows    = m_lower->MirLower(root);
            layouts = m_lower->Layouts();
            return true;
        }

        std::size_t indexOf(std::string_view function) const
        {
            return sema.table().findFunction(function);
        }

        const FunctionLayout& layoutOf(std::string_view function) const
        {
            return layouts[indexOf(function)];
        }

        //! The rows of one function: from its entry row up to and including the row before the next function starts.
        std::span<const Instruction> rowsOf(std::string_view function) const
        {
            const std::size_t start = layoutOf(function).entryRow;
            std::size_t       end   = rows.size();
            for (const auto& other : layouts)
            {
                if (other.entryRow > start && other.entryRow < end)
                {
                    end = other.entryRow;
                }
            }
            return std::span<const Instruction>(rows).subspan(start, end - start);
        }

        std::string messages() const
        {
            std::string out;
            for (const auto& d : diag.all())
            {
                out += d.message + "\n";
            }
            return out;
        }

    private:
        std::unique_ptr<Lexer>  m_lexer;
        std::unique_ptr<Parser> m_parser;
        std::unique_ptr<Lower>  m_lower;
    };

    std::vector<Op> opsOf(std::span<const Instruction> rows)
    {
        std::vector<Op> ops;
        for (const auto& r : rows)
        {
            ops.push_back(r.op);
        }
        return ops;
    }

    void expectConstInt(const Operand& o, std::int32_t v)
    {
        EXPECT_EQ(o.kind, OperandKind::Constant);
        EXPECT_EQ(o.type, ValueType::Int);
        ASSERT_TRUE(std::holds_alternative<std::int32_t>(o.value));
        EXPECT_EQ(std::get<std::int32_t>(o.value), v);
    }

    void expectTemp(const Operand& o, std::size_t slot)
    {
        EXPECT_EQ(o.kind, OperandKind::Temp);
        EXPECT_EQ(o.index, slot);
    }

    void expectLocal(const Operand& o, std::size_t slot)
    {
        EXPECT_EQ(o.kind, OperandKind::Local);
        EXPECT_EQ(o.index, slot);
    }

    //! Every label is placed exactly once and every jump targets a placed label.
    void expectLabelsConsistent(std::span<const Instruction> rows)
    {
        std::map<std::size_t, int> placed;
        for (const auto& r : rows)
        {
            if (r.op == Op::Label)
            {
                EXPECT_EQ(r.dest.kind, OperandKind::Label);
                ++placed[r.dest.index];
            }
        }
        for (const auto& [label, count] : placed)
        {
            EXPECT_EQ(count, 1) << "label " << label << " placed " << count << " times";
        }
        for (const auto& r : rows)
        {
            if (r.op == Op::Jump || r.op == Op::JumpIFF)
            {
                EXPECT_EQ(r.dest.kind, OperandKind::Label);
                EXPECT_TRUE(placed.contains(r.dest.index)) << "jump to label " << r.dest.index << " which is never placed";
            }
        }
    }

    //! Every temp that is read was written by an earlier row.
    void expectTempsWrittenBeforeRead(std::span<const Instruction> rows)
    {
        std::vector<std::size_t> written;
        auto                     isWritten = [&](std::size_t slot) {
            for (auto w : written)
            {
                if (w == slot)
                {
                    return true;
                }
            }
            return false;
        };

        for (std::size_t i = 0; i < rows.size(); ++i)
        {
            const auto& r = rows[i];
            for (const Operand* in : {&r.lhs, &r.rhs})
            {
                if (in->kind == OperandKind::Temp)
                {
                    EXPECT_TRUE(isWritten(in->index)) << "row " << i << " reads t" << in->index << " before it is written";
                }
            }
            if (r.dest.kind == OperandKind::Temp)
            {
                written.push_back(r.dest.index);
            }
            // each function has its own temps
            if (r.op == Op::Return && r.lhs.kind == OperandKind::None)
            {
                written.clear();
            }
        }
    }

} // namespace

// ------------------------------------------------------------ straight-line

TEST(Lower, EmptyProgramIsJustMainsReturn)
{
    Lowered l;
    ASSERT_TRUE(l.run("")) << l.messages();
    EXPECT_EQ(opsOf(l.rows), (std::vector<Op>{Op::Return}));
}

TEST(Lower, PrintsAConstantWithoutTemps)
{
    Lowered l;
    ASSERT_TRUE(l.run("print 42;")) << l.messages();

    ASSERT_EQ(opsOf(l.rows), (std::vector<Op>{Op::Print, Op::Return}));
    EXPECT_EQ(l.rows[0].dest.kind, OperandKind::None);
    expectConstInt(l.rows[0].lhs, 42);
}

TEST(Lower, AssignmentOfConstantIsOneStore)
{
    Lowered l;
    ASSERT_TRUE(l.run("x = 5;")) << l.messages();

    ASSERT_EQ(opsOf(l.rows), (std::vector<Op>{Op::Store, Op::Return}));
    expectLocal(l.rows[0].dest, 0);
    expectConstInt(l.rows[0].lhs, 5);
}

TEST(Lower, NestedExpressionIsFlattenedInnermostFirst)
{
    // x = 2 + 3 * 4   ->   mul t1 3 4 | add t2 2 t1 | store x t2
    Lowered l;
    ASSERT_TRUE(l.run("x = 2 + 3 * 4;")) << l.messages();

    ASSERT_EQ(opsOf(l.rows), (std::vector<Op>{Op::Mul, Op::Add, Op::Store, Op::Return}));

    expectTemp(l.rows[0].dest, 1);
    expectConstInt(l.rows[0].lhs, 3);
    expectConstInt(l.rows[0].rhs, 4);

    expectTemp(l.rows[1].dest, 2);
    expectConstInt(l.rows[1].lhs, 2);
    expectTemp(l.rows[1].rhs, 1);

    expectLocal(l.rows[2].dest, 0);
    expectTemp(l.rows[2].lhs, 2);
}

TEST(Lower, BinaryOperatorsAreLeftAssociative)
{
    // (2 + 7) - y : the add happens first and the sub reads its temp
    Lowered l;
    ASSERT_TRUE(l.run("y = 1; x = 2 + 7 - y;")) << l.messages();

    ASSERT_EQ(opsOf(l.rows), (std::vector<Op>{Op::Store, Op::Add, Op::Sub, Op::Store, Op::Return}));
    expectTemp(l.rows[2].lhs, l.rows[1].dest.index);
    expectLocal(l.rows[2].rhs, 0);
    expectLocal(l.rows[3].dest, 1);
}

TEST(Lower, TempsStartAfterMainsLocals)
{
    Lowered l;
    ASSERT_TRUE(l.run("a = 1; b = 2; c = a + b;")) << l.messages();

    for (const auto& r : l.rows)
    {
        if (r.dest.kind == OperandKind::Temp)
        {
            EXPECT_GE(r.dest.index, 3u) << "temp collides with a local slot";
        }
    }
}

TEST(Lower, EachOperatorMapsToItsOp)
{
    const std::vector<std::pair<std::string_view, Op>> cases = {
        { "print 1 + 2;",   Op::Add},
        { "print 1 - 2;",   Op::Sub},
        { "print 1 * 2;",   Op::Mul},
        { "print 1 / 2;",   Op::Div},
        { "print 7 % 2;",   Op::Mod},
        {"print 1 == 2;", Op::CmpEQ},
        {"print 1 != 2;", Op::CmpNE},
        { "print 1 < 2;", Op::CmpLT},
        {"print 1 <= 2;", Op::CmpLE},
        { "print 1 > 2;", Op::CmpGT},
        {"print 1 >= 2;", Op::CmpGE},
    };

    for (const auto& [src, op] : cases)
    {
        Lowered l;
        ASSERT_TRUE(l.run(src)) << src << "\n" << l.messages();
        ASSERT_EQ(opsOf(l.rows), (std::vector<Op>{op, Op::Print, Op::Return})) << src;
        expectTemp(l.rows[1].lhs, l.rows[0].dest.index);
    }
}

TEST(Lower, ComparisonResultIsBoolButInputsKeepTheirType)
{
    Lowered l;
    ASSERT_TRUE(l.run("print 1.5 < 2.5;")) << l.messages();

    ASSERT_EQ(l.rows[0].op, Op::CmpLT);
    EXPECT_EQ(l.rows[0].dest.type, ValueType::Bool);
    EXPECT_EQ(l.rows[0].lhs.type, ValueType::Float);
    EXPECT_EQ(l.rows[0].rhs.type, ValueType::Float);
}

TEST(Lower, NoOperatorIsSilentlyDropped)
{
    // every binary expression must produce a row; an empty operand means an unhandled operator
    for (std::string_view src : {"print true & false;", "print true | false;"})
    {
        Lowered l;
        ASSERT_TRUE(l.run(src)) << src << "\n" << l.messages();
        ASSERT_GE(l.rows.size(), 2u) << src;
        EXPECT_NE(l.rows[l.rows.size() - 2].lhs.kind, OperandKind::None) << src << ": print has nothing to print";
    }
}

// ------------------------------------------------------------ control flow

TEST(Lower, WhileLoopLowersToLabelsAndJumps)
{
    Lowered l;
    ASSERT_TRUE(l.run("i = 0; while (i < 3) { i = i + 1; }")) << l.messages();

    // store i 0 | label S | lt | jiff E | add | store | jump S | label E | return
    ASSERT_EQ(opsOf(l.rows), (std::vector<Op>{Op::Store, Op::Label, Op::CmpLT, Op::JumpIFF, Op::Add, Op::Store, Op::Jump, Op::Label, Op::Return}));

    const auto start = l.rows[1].dest.index;
    const auto exit  = l.rows[7].dest.index;
    EXPECT_NE(start, exit);
    EXPECT_EQ(l.rows[3].dest.index, exit);  // leave the loop when false
    EXPECT_EQ(l.rows[6].dest.index, start); // go back and re-check the condition
    expectTemp(l.rows[3].lhs, l.rows[2].dest.index);
    expectLabelsConsistent(l.rows);
}

TEST(Lower, IfWithoutElseUsesOneLabel)
{
    Lowered l;
    ASSERT_TRUE(l.run("x = 9; if (x > 5) { print 1; }")) << l.messages();

    ASSERT_EQ(opsOf(l.rows), (std::vector<Op>{Op::Store, Op::CmpGT, Op::JumpIFF, Op::Print, Op::Label, Op::Return}));
    EXPECT_EQ(l.rows[2].dest.index, l.rows[4].dest.index);
    expectLabelsConsistent(l.rows);
}

TEST(Lower, IfElseJumpsToElseThenSkipsIt)
{
    Lowered l;
    ASSERT_TRUE(l.run("x = 9; if (x > 5) { print 1; } else { print 2; }")) << l.messages();

    // store | gt | jiff ELSE | print 1 | jump END | label ELSE | print 2 | label END | return
    ASSERT_EQ(opsOf(l.rows), (std::vector<Op>{Op::Store, Op::CmpGT, Op::JumpIFF, Op::Print, Op::Jump, Op::Label, Op::Print, Op::Label, Op::Return}));

    const auto elseLabel = l.rows[5].dest.index;
    const auto endLabel  = l.rows[7].dest.index;
    EXPECT_NE(elseLabel, endLabel);
    EXPECT_EQ(l.rows[2].dest.index, elseLabel);
    EXPECT_EQ(l.rows[4].dest.index, endLabel);
    expectConstInt(l.rows[3].lhs, 1);
    expectConstInt(l.rows[6].lhs, 2);
    expectLabelsConsistent(l.rows);
}

TEST(Lower, LabelsAreUniqueAcrossStatements)
{
    Lowered l;
    ASSERT_TRUE(l.run("i = 0; while (i < 3) { i = i + 1; } if (i > 1) { print 1; } else { print 2; }"
                      "while (i > 0) { if (i == 2) { print i; } i = i - 1; }"))
        << l.messages();
    expectLabelsConsistent(l.rows);
}

// ------------------------------------------------------------ functions

TEST(Lower, FunctionBodyComesBeforeMainAndEndsWithReturn)
{
    Lowered l;
    ASSERT_TRUE(l.run("func add(a, b) { return a + b; } print add(1, 2);")) << l.messages();

    // add:  add t2 a b | return t2 | return
    // main: arg 1 | arg 2 | call t0 add | print t0 | return
    ASSERT_EQ(opsOf(l.rows), (std::vector<Op>{Op::Add, Op::Return, Op::Return, Op::Arg, Op::Arg, Op::Call, Op::Print, Op::Return}));

    // add's locals are a=0, b=1 so its first temp is slot 2
    expectTemp(l.rows[0].dest, 2);
    expectLocal(l.rows[0].lhs, 0);
    expectLocal(l.rows[0].rhs, 1);
    expectTemp(l.rows[1].lhs, 2);
    EXPECT_EQ(l.rows[2].lhs.kind, OperandKind::None);

    expectConstInt(l.rows[3].lhs, 1);
    expectConstInt(l.rows[4].lhs, 2);

    const auto& call = l.rows[5];
    EXPECT_EQ(call.lhs.kind, OperandKind::Func);
    EXPECT_EQ(call.lhs.index, l.indexOf("add"));
    // main has no locals so the counter restarts at 0
    expectTemp(call.dest, 0);
    EXPECT_EQ(call.dest.type, ValueType::Int);
    expectTemp(l.rows[6].lhs, 0);
}

TEST(Lower, VoidFunctionStillEndsWithReturn)
{
    Lowered l;
    ASSERT_TRUE(l.run("func greet() { print 1; } greet();")) << l.messages();

    ASSERT_EQ(opsOf(l.rows), (std::vector<Op>{Op::Print, Op::Return, Op::Call, Op::Return}));
    EXPECT_EQ(l.rows[2].dest.kind, OperandKind::None); // nothing comes back from a void call
    EXPECT_EQ(l.rows[2].lhs.index, l.indexOf("greet"));
}

TEST(Lower, NestedCallArgumentsStayNextToTheirCall)
{
    // f(1, g(2)): g's arg and call come first, then both of f's args, then f's call
    Lowered l;
    ASSERT_TRUE(l.run("func g(a) { return a; } func f(a, b) { return a + b; } print f(1, g(2));")) << l.messages();

    std::vector<Instruction> main;
    bool                     inMain  = false;
    std::size_t              returns = 0;
    for (const auto& r : l.rows)
    {
        if (inMain)
        {
            main.push_back(r);
        }
        // g and f each end with an explicit and an appended return
        if (r.op == Op::Return && ++returns == 4)
        {
            inMain = true;
        }
    }

    ASSERT_EQ(opsOf(main), (std::vector<Op>{Op::Arg, Op::Call, Op::Arg, Op::Arg, Op::Call, Op::Print, Op::Return}));
    expectConstInt(main[0].lhs, 2);
    EXPECT_EQ(main[1].lhs.index, l.indexOf("g"));
    expectConstInt(main[2].lhs, 1);
    expectTemp(main[3].lhs, main[1].dest.index);
    EXPECT_EQ(main[4].lhs.index, l.indexOf("f"));
}

TEST(Lower, FunctionsKeepTheirOwnTempNumbering)
{
    Lowered l;
    ASSERT_TRUE(l.run("func add3(a, b, c) { s = a + b; return s + c; }"
                      "func add31(a, b, c) { s = a + b; return s + c; }"
                      "x = add3(1, 2, 3) + add31(4, 5, 6);"))
        << l.messages();

    // both functions have 4 locals, so both use temps 4 and 5
    ASSERT_GE(l.rows.size(), 10u);
    for (std::size_t base : {0u, 5u})
    {
        expectTemp(l.rows[base + 0].dest, 4);
        expectLocal(l.rows[base + 1].dest, 3);
        expectTemp(l.rows[base + 2].dest, 5);
    }
    expectTempsWrittenBeforeRead(l.rows);
}

TEST(Lower, CallsBeforeDeclarationAreLoweredToo)
{
    Lowered l;
    ASSERT_TRUE(l.run("print twice(4); func twice(n) { return n * 2; }")) << l.messages();

    bool sawCall = false;
    for (const auto& r : l.rows)
    {
        if (r.op == Op::Call)
        {
            sawCall = true;
            EXPECT_EQ(r.lhs.kind, OperandKind::Func);
            EXPECT_EQ(r.lhs.index, l.indexOf("twice"));
        }
    }
    EXPECT_TRUE(sawCall);
}

// ------------------------------------------------------------ unary

TEST(Lower, NotHasOneInputAndATemp)
{
    Lowered l;
    ASSERT_TRUE(l.run("print !true;")) << l.messages();

    ASSERT_EQ(opsOf(l.rows), (std::vector<Op>{Op::CmpN, Op::Print, Op::Return}));
    EXPECT_EQ(l.rows[0].dest.kind, OperandKind::Temp);
    EXPECT_EQ(l.rows[0].lhs.kind, OperandKind::Constant);
    EXPECT_EQ(l.rows[0].rhs.kind, OperandKind::None);
}

TEST(Lower, NegationDoesNotLeaveAnOperandMissing)
{
    // whatever op negation lowers to, a two-input op like Sub or Mul needs both inputs
    Lowered l;
    ASSERT_TRUE(l.run("x = 3; print -x;")) << l.messages();

    const auto& neg = l.rows[1];
    EXPECT_EQ(neg.dest.kind, OperandKind::Temp);
    EXPECT_NE(neg.lhs.kind, OperandKind::None);

    const bool twoInputs = neg.op == Op::Add || neg.op == Op::Sub || neg.op == Op::Mul || neg.op == Op::Div;
    if (twoInputs)
    {
        EXPECT_NE(neg.rhs.kind, OperandKind::None) << "two-input op with only one input";
    }

    // x itself must be one of the inputs
    const bool readsX = (neg.lhs.kind == OperandKind::Local && neg.lhs.index == 0) || (neg.rhs.kind == OperandKind::Local && neg.rhs.index == 0);
    EXPECT_TRUE(readsX);
}

// ------------------------------------------------------------ layouts

TEST(LowerLayout, OneLayoutPerFunctionIncludingMain)
{
    Lowered l;
    ASSERT_TRUE(l.run("func f() { print 1; } func g() { print 2; } f(); g();")) << l.messages();
    EXPECT_EQ(l.layouts.size(), l.sema.table().functions().size());
}

TEST(LowerLayout, MainEntersAtItsFirstRow)
{
    Lowered l;
    ASSERT_TRUE(l.run("print 1;")) << l.messages();

    // only main exists, so it starts at the very first row
    EXPECT_EQ(l.layoutOf("@main").entryRow, 0u);
}

TEST(LowerLayout, EmptyProgramEntersAtMainsReturn)
{
    Lowered l;
    ASSERT_TRUE(l.run("")) << l.messages();

    const auto& main = l.layoutOf("@main");
    ASSERT_LT(main.entryRow, l.rows.size()) << "main's entry row is past the end of the instructions";
    EXPECT_EQ(l.rows[main.entryRow].op, Op::Return);
    EXPECT_EQ(main.slotCount, 0u);
}

TEST(LowerLayout, EntryRowsPointAtEachFunctionsFirstRow)
{
    Lowered l;
    ASSERT_TRUE(l.run("func add(a, b) { return a + b; } print add(1, 2);")) << l.messages();

    // add: rows 0-2 | main: rows 3-7
    EXPECT_EQ(l.layoutOf("add").entryRow, 0u);
    EXPECT_EQ(l.layoutOf("@main").entryRow, 3u);
    EXPECT_EQ(l.rows[l.layoutOf("@main").entryRow].op, Op::Arg);
}

TEST(LowerLayout, EveryEntryRowIsInRangeAndFollowsAReturn)
{
    Lowered l;
    ASSERT_TRUE(l.run("x = 1;"
                      "func f(a) { if (a > 0) { return 1; } }"
                      "func g(a, b) { while (a < b) { a = a + 1; } return a; }"
                      "print f(x) + g(x, 5);"))
        << l.messages();

    for (std::size_t i = 0; i < l.layouts.size(); ++i)
    {
        const auto entry = l.layouts[i].entryRow;
        ASSERT_LT(entry, l.rows.size()) << "function " << i << " enters past the end";
        if (entry > 0)
        {
            // the previous function must have finished before this one starts
            EXPECT_EQ(l.rows[entry - 1].op, Op::Return) << "function " << i;
        }
    }
}

TEST(LowerLayout, EachFunctionsRowsEndWithReturn)
{
    Lowered l;
    ASSERT_TRUE(l.run("func f(a) { if (a > 0) { return 1; } } func g() { print 2; } print f(1); g();")) << l.messages();

    for (std::string_view name : {"f", "g", "@main"})
    {
        const auto body = l.rowsOf(name);
        ASSERT_FALSE(body.empty()) << name;
        EXPECT_EQ(body.back().op, Op::Return) << name;
    }
}

TEST(LowerLayout, SlotCountIsLocalsPlusTemps)
{
    Lowered l;
    ASSERT_TRUE(l.run("func add3(a, b, c) { s = a + b; return s + c; } x = add3(1, 2, 3);")) << l.messages();

    // add3: a, b, c, s + t4, t5
    EXPECT_EQ(l.layoutOf("add3").slotCount, 6u);
    // main: x + the call's temp
    EXPECT_EQ(l.layoutOf("@main").slotCount, 2u);
}

TEST(LowerLayout, VoidCallTakesNoSlot)
{
    Lowered l;
    ASSERT_TRUE(l.run("func greet() { print 1; } greet();")) << l.messages();

    EXPECT_EQ(l.layoutOf("greet").slotCount, 0u);
    EXPECT_EQ(l.layoutOf("@main").slotCount, 0u);
}

TEST(LowerLayout, EverySlotUsedFitsInTheFrame)
{
    Lowered l;
    ASSERT_TRUE(l.run("func f(a, b) { c = a * b; while (c > 10) { c = c - a; } return c + 1; }"
                      "x = 3; y = f(x, 7); print y + x * 2;"))
        << l.messages();

    for (std::string_view name : {"f", "@main"})
    {
        const auto slots = l.layoutOf(name).slotCount;
        for (const auto& r : l.rowsOf(name))
        {
            for (const Operand* o : {&r.dest, &r.lhs, &r.rhs})
            {
                if (o->kind == OperandKind::Temp || o->kind == OperandKind::Local)
                {
                    EXPECT_LT(o->index, slots) << name << " uses slot " << o->index << " but its frame has " << slots;
                }
            }
        }
    }
}

TEST(LowerLayout, FunctionsDeclaredAfterUseStillGetTheirOwnEntry)
{
    Lowered l;
    ASSERT_TRUE(l.run("print twice(4); func twice(n) { return n * 2; }")) << l.messages();

    const auto body = l.rowsOf("twice");
    ASSERT_FALSE(body.empty());
    EXPECT_EQ(body.front().op, Op::Mul);
    EXPECT_EQ(l.rows[l.layoutOf("@main").entryRow].op, Op::Arg);
}
