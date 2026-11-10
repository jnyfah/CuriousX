#pragma once
#include <cstddef>
#include <span>
#include <string_view>
#include <vector>
#include "helpers/type.hpp"
#include "lower/lower.hpp"

namespace cx
{
    class Asm
    {

    public:
        Asm(std::span<const Instruction> instr);

        void TargetLower(std::span<const FunctionLayout> layout, const std::size_t main_index);

    private:
        size_t                       m_argCount   = 0;
        size_t                       m_frameCount = 0;
        size_t                       m_entry      = 0;
        std::span<const Instruction> m_instr;

        const char*                  match(const Op& op, const ValueType& type);
        void                         load(const Operand& operand, const size_t reg);
        void                         store(const Operand& operand, const size_t reg);
        void                         emitBinaryOp(const Instruction& instr);

        Instruction                  peekInstr();
        // change this name InstEmit
        void                         emitInstruction(const Instruction& instr);
    };
} // namespace cx

// also we save in file but lets cout first
// add support for float
// add const and & where necessary
// optimize comp + jump if to be 1 instruction

// format
// reg allocation
// topdirective
// file
