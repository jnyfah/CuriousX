#include "lower/lower.hpp"
#include <cstddef>
#include <vector>
#include "helpers/type.hpp"
#include "lexer/token.hpp"
#include "lower.hpp"
#include "parser/arena.hpp"
#include "parser/node.hpp"

namespace cx
{

    Lower::Lower(Arena& arena, const SymbolTable& sym) : m_arena(arena), m_sym(sym), m_nextSlot(0), m_labelcount(0), m_index(0) {}

    std::span<Instruction> Lower::MirLower(ProgramNode* root)
    {
        m_layouts.resize(m_sym.functions().size());

        // Two Passes like the sematic analysis
        // work on function declarations first
        for (Node* statement : root->statements)
        {
            if (statement->kind == NodeKind::FuncDecl)
            {
                FuncDclInstr(static_cast<FuncNode*>(statement));
            }
        }

        m_index          = m_sym.findFunction("@main");
        const auto& func = m_sym.functions()[m_index];
        ResetCounter(func.locals.size());
        const auto instrsize = m_instr.size();

        for (Node* statement : root->statements)
        {

            StmtInstr(statement);
        }

        m_instr.emplace_back(Op::Return);
        m_layouts[m_index] = {instrsize, m_nextSlot, m_arena.copyOf(func.paramTypes), func.returnType};

        return m_arena.copyOf(m_instr);
    }

    std::size_t Lower::MainIndex()
    {
        return m_index;
    }

    std::span<FunctionLayout> Lower::Layouts()
    {
        return m_arena.copyOf(m_layouts);
    }

    std::size_t Lower::Counter()
    {
        return m_nextSlot++;
    }

    std::size_t Lower::LabelCounter()
    {
        m_labelcount += 1;
        return m_labelcount;
    }

    void Lower::ResetCounter(std::size_t count)
    {
        m_nextSlot = count;
    }

    void Lower::StmtInstr(Node* node)
    {
        switch (node->kind)
        {
            case NodeKind::IfStmt:
                IfInstr(static_cast<IfNode*>(node));
                break;
            case NodeKind::ReturnStmt:
                ReturnInstr(static_cast<ReturnNode*>(node));
                break;
            case NodeKind::WhileStmt:
                WhileInstr(static_cast<WhileNode*>(node));
                break;
            case NodeKind::Print:
                PrintInstr(static_cast<PrintNode*>(node));
                break;
            case NodeKind::FuncDecl:
                // already lowered in MirLower's first pass
                break;
            default:
                ExprInstr(node);
        }
    }

    Operand Lower::ExprInstr(Node* node)
    {
        switch (node->kind)
        {
            case NodeKind::Number:
            case NodeKind::String:
            case NodeKind::Bool:
                return Operand{OperandKind::Constant, node->valuetype, valueOf(node), node->localIndex};
            case NodeKind::Identifier:
                return Operand{OperandKind::Local, node->valuetype, valueOf(node), node->localIndex};
            case NodeKind::BinaryExpr:
                return BinaryInstr(static_cast<BinaryNode*>(node));
            case NodeKind::UnaryExpr:
                return UnaryInstr(static_cast<UnaryNode*>(node));
            case NodeKind::FuncCall:
                return CallInstr(static_cast<CallNode*>(node));
            case NodeKind::Error:
            default:
                return Operand{};
        }
        return Operand{};
    }

    Operand Lower::BinaryInstr(BinaryNode* node)
    {
        if (node->token.type == TokenType::Assign)
        {
            const auto right = ExprInstr(node->right);
            const auto left  = ExprInstr(node->left);

            // this is a store, so storing right into left
            m_instr.emplace_back(Op::Store, left, right);
            return right;
        }

        const auto      left  = ExprInstr(node->left);
        const auto      right = ExprInstr(node->right);
        const TokenType type  = node->token.type;
        Operand         operand;
        Op              op;

        switch (type)
        {
            case cx::TokenType::Plus:
                op = Op::Add;
                break;
            case cx::TokenType::Minus:
                op = Op::Sub;
                break;
            case cx::TokenType::Divide:
                op = Op::Div;
                break;
            case cx::TokenType::Multiply:
                op = Op::Mul;
                break;
            case cx::TokenType::Equal:
                op = Op::CmpEQ;
                break;
            case cx::TokenType::GreaterEqual:
                op = Op::CmpGE;
                break;
            case cx::TokenType::LessEqual:
                op = Op::CmpLE;
                break;
            case cx::TokenType::NotEqual:
                op = Op::CmpNE;
                break;
            case cx::TokenType::Less:
                op = Op::CmpLT;
                break;
            case cx::TokenType::Greater:
                op = Op::CmpGT;
                break;
            case cx::TokenType::And:
                op = Op::And;
                break;
            case cx::TokenType::Or:
                op = Op::Or;
                break;
            case cx::TokenType::Percent:
                op = Op::Mod;
                break;
            default:
                return operand;
        }

        operand = {OperandKind::Temp, node->valuetype, std::monostate{}, Counter()};
        m_instr.emplace_back(op, operand, left, right);
        return operand;
    }

    Operand Lower::UnaryInstr(UnaryNode* node)
    {
        const auto value = ExprInstr(node->operand);
        const auto dest  = Operand{OperandKind::Temp, node->valuetype, std::monostate{}, Counter()};

        if (node->token.type == TokenType::Minus)
        {
            // negation is 0 - x, with a zero of the same type as x
            const ConstValue zero = node->valuetype == ValueType::Float ? ConstValue{0.0} : ConstValue{std::int32_t{0}};
            const auto       lhs  = Operand{OperandKind::Constant, node->valuetype, zero, 0};
            m_instr.emplace_back(Op::Sub, dest, lhs, value);
        }
        else
        {
            // `!` takes a single input
            m_instr.emplace_back(Op::CmpN, dest, value);
        }

        return dest;
    }

    Operand Lower::Label()
    {
        return Operand{OperandKind::Label, ValueType::Unknown, std::monostate{}, LabelCounter()};
    }

    void Lower::IfInstr(IfNode* node)
    {
        const auto cond       = ExprInstr(node->condition);

        const auto else_label = Label();
        m_instr.emplace_back(Op::JumpIFF, else_label, cond);

        // then
        for (auto n : node->then)
        {
            StmtInstr(n);
        }

        // else
        if (!node->nelse.empty())
        {
            const auto end_label = Label();
            m_instr.emplace_back(Op::Jump, end_label);
            m_instr.emplace_back(Op::Label, else_label);

            for (auto n : node->nelse)
            {
                StmtInstr(n);
            }

            m_instr.emplace_back(Op::Label, end_label);
        }
        else
        {
            m_instr.emplace_back(Op::Label, else_label);
        }
    }

    void Lower::WhileInstr(WhileNode* node)
    {
        const auto loop_start = Label();
        const auto loop_end   = Label();

        m_instr.emplace_back(Op::Label, loop_start);

        const auto cond = ExprInstr(node->condition);
        m_instr.emplace_back(Op::JumpIFF, loop_end, cond);

        // then
        for (auto n : node->loop)
        {
            StmtInstr(n);
        }

        m_instr.emplace_back(Op::Jump, loop_start);
        m_instr.emplace_back(Op::Label, loop_end);
    }

    void Lower::PrintInstr(PrintNode* node)
    {
        const auto expr = ExprInstr(node->expression);
        m_instr.emplace_back(Op::Print, Operand{}, expr);
    }

    Operand Lower::CallInstr(CallNode* node)
    {
        const auto           index = m_sym.findFunction(node->callee->token.value);

        std::vector<Operand> args;
        args.reserve(node->arguments.size());

        for (auto* arg : node->arguments)
        {
            args.emplace_back(ExprInstr(arg));
        }

        for (const auto& arg : args)
        {
            m_instr.emplace_back(Op::Arg, Operand{}, arg);
        }

        //  non-void calls.
        Operand dest{};

        if (node->valuetype != ValueType::Void)
        {
            dest = Operand{OperandKind::Temp, node->valuetype, std::monostate{}, Counter()};
        }

        const auto func = Operand{OperandKind::Func, ValueType::Unknown, std::monostate{}, index};
        m_instr.emplace_back(Op::Call, dest, func);

        return dest;
    }

    void Lower::FuncDclInstr(FuncNode* node)
    {
        // reset temp counter for all new function, and temp number should start after all locals in the function
        auto        index = m_sym.findFunction(node->name->token.value);
        const auto& func  = m_sym.functions()[index];
        ResetCounter(func.locals.size());

        const auto instrsize = m_instr.size();

        for (Node* statement : node->body)
        {
            StmtInstr(statement);
        }

        // the body may not end with a return on every path
        m_instr.emplace_back(Op::Return);
        // map the index to the function to to its start so execution never runs into the next function's rows
        m_layouts[index] = {instrsize, m_nextSlot, m_arena.copyOf(func.paramTypes), func.returnType};
    }

    void Lower::ReturnInstr(ReturnNode* node)
    {
        const auto returned = node->expression ? ExprInstr(node->expression) : Operand{};
        m_instr.emplace_back(Op::Return, Operand{}, returned);
    }

} // namespace cx
