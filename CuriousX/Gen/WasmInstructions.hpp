#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

enum class WasmInstruction
{
    I32Const, 
    F32Const, 

    // Integer arithmetic
    I32Add,
    I32Sub,
    I32Mul,
    I32DivS,

    // Float arithmetic
    F32Add,
    F32Sub,
    F32Mul,
    F32Div,

    // Comparisons (integers)
    I32Eq,  
    I32Ne, 
    I32LtS, 
    I32GtS, 
    I32LeS, 
    I32GeS, 

    // Comparisons (floats)
    F32Eq, 
    F32Ne, 
    F32Lt, 
    F32Gt, 
    F32Le, 
    F32Ge, 

    // Boolean values
    I32True,  
    I32False, 

    // Local variables
    LocalGet,
    LocalSet,

    // Control flow
    If,
    Else,
    End,
    CallPrint
};

struct WasmInstructionWithData
{
    WasmInstruction instruction;
    std::string data;
    bool has_data; 

    WasmInstructionWithData(WasmInstruction instr) : instruction(instr), has_data(false) {}

    WasmInstructionWithData(WasmInstruction instr, std::string value)
        : instruction(instr), data(value), has_data(true)
    {
    }
};

inline std::string instructionToString(const WasmInstructionWithData& instr)
{
    switch (instr.instruction)
    {
    case WasmInstruction::I32Const:
        return "i32.const " + instr.data;
    case WasmInstruction::F32Const:
        return "f32.const " + instr.data;

    // Integer arithmetic
    case WasmInstruction::I32Add:
        return "i32.add";
    case WasmInstruction::I32Sub:
        return "i32.sub";
    case WasmInstruction::I32Mul:
        return "i32.mul";
    case WasmInstruction::I32DivS:
        return "i32.div_s";

    // Float arithmetic
    case WasmInstruction::F32Add:
        return "f32.add";
    case WasmInstruction::F32Sub:
        return "f32.sub";
    case WasmInstruction::F32Mul:
        return "f32.mul";
    case WasmInstruction::F32Div:
        return "f32.div";

    // Integer comparisons
    case WasmInstruction::I32Eq:
        return "i32.eq";
    case WasmInstruction::I32Ne:
        return "i32.ne";
    case WasmInstruction::I32LtS:
        return "i32.lt_s";
    case WasmInstruction::I32GtS:
        return "i32.gt_s";
    case WasmInstruction::I32LeS:
        return "i32.le_s";
    case WasmInstruction::I32GeS:
        return "i32.ge_s";

    // Float comparisons
    case WasmInstruction::F32Eq:
        return "f32.eq";
    case WasmInstruction::F32Ne:
        return "f32.ne";
    case WasmInstruction::F32Lt:
        return "f32.lt";
    case WasmInstruction::F32Gt:
        return "f32.gt";
    case WasmInstruction::F32Le:
        return "f32.le";
    case WasmInstruction::F32Ge:
        return "f32.ge";

    // Boolean values
    case WasmInstruction::I32True:
        return "i32.const True"; 
    case WasmInstruction::I32False:
        return "i32.const False"; 

    // Local variables
    case WasmInstruction::LocalGet:
        return "local.get " + instr.data;
    case WasmInstruction::LocalSet:
        return "local.set " + instr.data;

    // Control flow
    case WasmInstruction::If:
        return "if";
    case WasmInstruction::Else:
        return "else";
    case WasmInstruction::End:
        return "end";

    // Print function
    case WasmInstruction::CallPrint:
        return "call $print";

    default:
        // throw std::runtime_error("Unknown instruction");
        break;
    }
    return "<<BAD TYPE>>";
}
