#include "asm.h"
#include <cstddef>
#include <iostream>
#include "helpers/type.hpp"
#include "lower/lower.hpp"

namespace cx
{

    Asm::Asm(std::span<const Instruction> instr) : m_instr(instr) {}

    void Asm::TargetLower(std::span<const FunctionLayout> layout, const std::size_t main_index)
    {
        // print all fcn first
        for (size_t m_index = 0; m_index < layout.size(); m_index++)
        {
            auto func = layout[m_index];
            std::cout << std::endl;
            (m_index == main_index) ? std::cout << "main:  \n" : std::cout << "func" << m_index << ": \n";

            // set this function's count
            m_frameCount                = func.slotCount;

            // sp down by frame size: the slots plus one place for ra, rounded up to a multiple of 16
            const std::size_t frameSize = (((m_frameCount + 1) * 8 + 15) / 16) * 16;
            std::cout << "addi sp, sp, -" << frameSize << "\n";

            // usave all func parameters
            for (size_t reg = 0; reg < func.paramTypes.size(); reg++)
            {
                std::cout << "sd a" << reg << ", " << (reg * 8) << "(sp)\n";
            }

            // save return
            std::cout << "sd ra, " << (func.slotCount * 8) << "(sp)\n";
            m_entry           = func.entryRow;
            size_t next_index = 0;

            // what if we are at the last
            (m_index == layout.size() - 1) ? next_index = m_instr.size() : next_index = layout[m_index + 1].entryRow;
            while (m_entry < next_index)
            {
                emitInstruction(m_instr[m_entry]);
                m_entry++;
            }
        }
    }

    void Asm::emitBinaryOp(const Instruction& instr)
    {
        const char* ch = match(instr.op, instr.dest.type);
        load(instr.lhs, 0);
        load(instr.rhs, 1);
        std::cout << ch << " t0, t0, t1\n";
        store(instr.dest, 0);
    }

    Instruction Asm::peekInstr()
    {
        const size_t i = m_entry + 1;
        return m_instr[i];
    }

    void Asm::emitInstruction(const Instruction& instr)
    {
        switch (instr.op)
        {
            case Op::Add:
            case Op::Sub:
            case Op::Mul:
            case Op::Div:
            {
                emitBinaryOp(instr);
            }
            break;
            case Op::CmpEQ:
            {
                const char* ch = match(instr.op, instr.dest.type);
                load(instr.lhs, 0);
                load(instr.rhs, 1);
                auto next = peekInstr();
                if (next.op == Op::JumpIFF)
                {
                    std::cout << "bne t0, t1, LABEL" << next.dest.index << "\n";
                    m_entry++;
                    break;
                }
                std::cout << ch << " t0, t0, t1\n";
                std::cout << "seqz t0, t0" << "\n";
                store(instr.dest, 0);
            }
            break;
            case Op::JumpIFF:
            {
                load(instr.lhs, 0);
                std::cout << "beqz t0, LABEL" << instr.dest.index << "\n";
            }
            break;
            case Op::Jump:
            {
                std::cout << std::endl;
                std::cout << "j LABEL" << instr.dest.index << "\n";
            }
            break;
            case Op::Label:
            {
                std::cout << std::endl;
                std::cout << "LABEL" << instr.dest.index << ": \n";
            }
            break;
            case Op::CmpNE:
            {
                const char* ch = match(instr.op, instr.dest.type);
                load(instr.lhs, 0);
                load(instr.rhs, 1);
                auto next = peekInstr();
                if (next.op == Op::JumpIFF)
                {
                    std::cout << "beq t0, t1, LABEL" << next.dest.index << "\n";
                    m_entry++;
                    break;
                }
                std::cout << ch << " t0, t0, t1\n";
                std::cout << "snez t0, t0" << "\n";
                store(instr.dest, 0);
            }
            break;
            case Op::CmpLT:
            {
                const char* ch = match(instr.op, instr.dest.type);
                load(instr.lhs, 0);
                load(instr.rhs, 1);

                auto next = peekInstr();
                if (next.op == Op::JumpIFF)
                {
                    std::cout << "bge t0, t1, LABEL" << next.dest.index << "\n";
                    m_entry++;
                    break;
                }
                std::cout << ch << " t0, t0, t1\n";
                store(instr.dest, 0);
            }
            break;
            case Op::CmpGT:
            {
                const char* ch = match(instr.op, instr.dest.type);
                load(instr.lhs, 0);
                load(instr.rhs, 1);
                auto next = peekInstr();
                if (next.op == Op::JumpIFF)
                {
                    std::cout << "bge t1, t0, LABEL" << next.dest.index << "\n";
                    m_entry++;
                    break;
                }
                std::cout << ch << " t0, t1, t0\n";
                store(instr.dest, 0);
            }
            break;
            case Op::CmpGE:
            {
                // a >= b  is  !(a < b)
                load(instr.lhs, 0);
                load(instr.rhs, 1);
                auto next = peekInstr();
                if (next.op == Op::JumpIFF)
                {
                    std::cout << "blt t0, t1, LABEL" << next.dest.index << "\n";
                    m_entry++;
                    break;
                }
                std::cout << "slt t0, t0, t1\n";
                std::cout << "xori t0, t0, 1\n";
                store(instr.dest, 0);
            }
            break;
            case Op::CmpLE:
            {
                // a <= b  is  !(b < a)
                load(instr.lhs, 0);
                load(instr.rhs, 1);
                auto next = peekInstr();
                if (next.op == Op::JumpIFF)
                {
                    std::cout << "blt t1, t0, LABEL" << next.dest.index << "\n";
                    m_entry++;
                    break;
                }
                std::cout << "slt t0, t1, t0\n";
                std::cout << "xori t0, t0, 1\n";
                store(instr.dest, 0);
            }
            break;
            case Op::CmpN:
            {
                // !a flips a 0/1 bool; CmpN has only an lhs
                load(instr.lhs, 0);
                std::cout << "xori t0, t0, 1\n";
                store(instr.dest, 0);
            }
            break;
            case Op::And:
            {
                // bools are 0/1, so bitwise and gives the logical result
                load(instr.lhs, 0);
                load(instr.rhs, 1);
                std::cout << "and t0, t0, t1\n";
                store(instr.dest, 0);
            }
            break;
            case Op::Or:
            {
                load(instr.lhs, 0);
                load(instr.rhs, 1);
                std::cout << "or t0, t0, t1\n";
                store(instr.dest, 0);
            }
            break;
            case Op::Mod:
            {
                // remw: 32-bit remainder, same as the int arithmetic
                load(instr.lhs, 0);
                load(instr.rhs, 1);
                std::cout << "remw t0, t0, t1\n";
                store(instr.dest, 0);
            }
            break;
            case Op::Arg:
            {
                // the n-th Arg row before a Call goes into register a<n>
                // only a0..a7 exist; more than 8 args would need the stack
                // todo dont allow more than 8 args
                load(instr.lhs, 0);
                std::cout << "mv a" << m_argCount << ", t0\n";
                m_argCount++;
            }
            break;
            case Op::Store:
            {
                load(instr.lhs, 0);
                store(instr.dest, 0);
            }
            break;
            case Op::Call:
            {
                std::cout << "call func" << instr.lhs.index << "\n";
                m_argCount = 0;

                if (instr.dest.kind != OperandKind::None)
                {
                    std::cout << "mv t0, a0\n";
                    store(instr.dest, 0);
                }
            }
            break;
            case Op::Return:
            {
                if (instr.lhs.kind != OperandKind::None)
                {
                    load(instr.lhs, 0);
                    std::cout << "mv a0, t0\n";
                }
                std::cout << "ld " << "ra, " << (m_frameCount * 8) << "(sp)\n";
                const std::size_t frameSize = (((m_frameCount + 1) * 8 + 15) / 16) * 16;

                std::cout << "addi sp, sp, " << frameSize << "\n";
                std::cout << "ret" << std::endl;
            }

            break;
            default:
                std::cout << "lala \n";
        }
    }

    void Asm::load(const Operand& operand, const size_t reg)
    {
        if (operand.kind == OperandKind::Constant)
        {
            std::visit(
                [&](const auto& v) {
                    using T = std::decay_t<decltype(v)>;

                    if constexpr (std::is_same_v<T, std::monostate>)
                    {
                        // No value
                    }
                    else
                    {
                        std::cout << "li t" << reg << ", " << v << "\n";
                    }
                },
                operand.value);
        }
        else
        {
            std::cout << "ld " << "t" << reg << ", " << (operand.index * 8) << "(sp)\n";
        }
    }

    void Asm::store(const Operand& operand, const size_t reg)
    {

        std::cout << "sd t" << reg << ", " << (operand.index * 8) << "(sp)\n";
    }

    const char* Asm::match(const Op& op, const ValueType& type)
    {
        switch (op)
        {

            case Op::Add:
                return type == ValueType::Int ? "addw" : "fadd.s";
            case Op::Sub:
                return type == ValueType::Int ? "subw" : "fsub.s";
            case Op::Mul:
                return type == ValueType::Int ? "mulw" : "fmul.s";
            case Op::Div:
                return type == ValueType::Int ? "divw" : "fdiv.s";
            case Op::CmpEQ:
            case Op::CmpNE:
                return "subw";
            case Op::CmpLT:
            case Op::CmpGT:
                return "slt";
            default:
                return "";
        }
    }

} // namespace cx