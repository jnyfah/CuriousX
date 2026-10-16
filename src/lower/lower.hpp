#pragma once
#include <cstddef>
#include <string_view>
#include <variant>
#include <vector>
#include "helpers/type.hpp"
#include "parser/arena.hpp"
#include "parser/node.hpp"
#include "sema/symboltable.hpp"

namespace cx
{
    using ConstValue = std::variant<std::monostate, std::int32_t, double, bool, std::string_view>;

    enum class Op
    {
        Add,
        Sub,
        Mul,
        Div,

        CmpEQ,
        CmpNE,
        CmpLT,
        CmpLE,
        CmpGT,
        CmpGE,
        CmpN,

        And,
        Or,
        Mod,
        Arg,

        Store,
        Load,
        Return,
        Call,
        Print,

        Label,
        Jump,
        JumpIFF,
        None,
    };

    enum class OperandKind
    {
        None,     //!< unused slot
        Constant, //!< value known at compile time
        Temp,     //!< compiler-made scratch box: t1, t2, ...
        Local,    //!< a named variable, by its slot in the function
        Label,
        Func,
    };

    struct Operand
    {
        OperandKind kind = OperandKind::None;
        ValueType   type = ValueType::Unknown;
        ConstValue  value;     //!< used when kind == Constant
        std::size_t index = 0; //!< temp number when Temp, local slot when Local
    };

    struct Instruction
    {
        Op      op   = Op::None;
        Operand dest = {};
        Operand lhs  = {};
        Operand rhs  = {};
    };

    struct FunctionLayout
    {
        size_t               entryRow  = 0; //!< row in the instruction list where this function starts
        size_t               slotCount = 0; //!< total no of var(local + IR temp)
        std::span<ValueType> paramTypes;
        ValueType            returnType = ValueType::Unknown;
    };

    //!< Intermidiate Representation
    // Call Order: MirLower first, then Layouts/MainIndex
    class Lower
    {
    public:
        Lower(Arena& arena, const SymbolTable& sym);

        std::span<Instruction>    MirLower(ProgramNode* root);
        std::span<FunctionLayout> Layouts();
        std::size_t               MainIndex();

    private:
        Arena&                      m_arena;
        const SymbolTable&          m_sym;

        std::size_t                 m_nextSlot; //<! the next free slot, reset to the locals count for each function
        std::size_t                 m_labelcount;
        std::size_t                 m_index;

        std::vector<Instruction>    m_instr;
        // each index represents a function, and index can be found in m_instr's call of the func
        std::vector<FunctionLayout> m_layouts;

        void                        ResetCounter(std::size_t count);
        size_t                      LabelCounter();
        std::size_t                 Counter();

        void                        StmtInstr(Node* node);
        void                        IfInstr(IfNode* node);
        void                        WhileInstr(WhileNode* node);
        void                        ReturnInstr(ReturnNode* node);
        void                        PrintInstr(PrintNode* node);
        void                        FuncDclInstr(FuncNode* node);

        Operand                     BinaryInstr(BinaryNode* node);
        Operand                     UnaryInstr(UnaryNode* node);
        Operand                     CallInstr(CallNode* node);
        Operand                     ExprInstr(Node* node);
        Operand                     Label();
    };

    inline ConstValue valueOf(const Node* node)
    {
        switch (node->kind)
        {
            case NodeKind::Number:
            {
                if (node->valuetype == ValueType::Float)
                {
                    double d = 0.0;
                    std::from_chars(node->token.value.data(), node->token.value.data() + node->token.value.size(), d);
                    return d;
                }

                std::int32_t i = 0;
                std::from_chars(node->token.value.data(), node->token.value.data() + node->token.value.size(), i);
                return i;
            }
            case NodeKind::Bool:
                return node->token.value == "true";
            case NodeKind::String:
                return std::string_view{node->token.value};
            default:
                return std::monostate{};
        }
    }

} // namespace cx
