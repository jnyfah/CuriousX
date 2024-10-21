#include "Codegen.hpp"

void WasmGen::generate(const ASTNode& node)
{
    switch (node.getType())
    {
    case NodeType::BinaryOperation:
        generateBinaryOp(static_cast<const BinaryNode&>(node));
        break;
    case NodeType::ConditionalOperation:
        generateConditional(static_cast<const ConditionalNode&>(node));
        break;
    case NodeType::BlockOperation:
        generateBlock(static_cast<const TreeNode&>(node));
        break;
    default:
        // Handle unexpected node types
        break;
    }
}

void WasmGen::generateBinaryOp(const BinaryNode& node)
{
    if (node.token.type == LexerTokenType::AssignToken)
    {
        generateExpression(static_cast<const BinaryNode&>(*node.right));
        int localIndex = getOrCreateLocalIndex(node.left->token.value);
        addInstruction(WasmInstructionWithData(WasmInstruction::LocalSet, std::to_string(localIndex)));
    }
    else
    {
        generateExpression(node);
    }
}

void WasmGen::generateExpression(const BinaryNode& node)
{
    if (node.left)
        generateExpression(static_cast<const BinaryNode&>(*node.left));
    if (node.right)
        generateExpression(static_cast<const BinaryNode&>(*node.right));

    bool isFloatOperation = isFloatType(static_cast<const BinaryNode&>(node));

    switch (node.token.type)
    {
    case LexerTokenType::PlusToken:
        addInstruction(WasmInstructionWithData(isFloatOperation ? WasmInstruction::F32Add : WasmInstruction::I32Add));
        break;
    case LexerTokenType::MinusToken:
        addInstruction(WasmInstructionWithData(isFloatOperation ? WasmInstruction::F32Sub : WasmInstruction::I32Sub));
        break;
    case LexerTokenType::MultiplyToken:
        addInstruction(WasmInstructionWithData(isFloatOperation ? WasmInstruction::F32Mul : WasmInstruction::I32Mul));
        break;
    case LexerTokenType::DivideToken:
        addInstruction(WasmInstructionWithData(isFloatOperation ? WasmInstruction::F32Div : WasmInstruction::I32DivS));
        break;
    case LexerTokenType::IntToken:
        addInstruction(WasmInstructionWithData(WasmInstruction::I32Const, std::string(node.token.value)));
        break;
    case LexerTokenType::FloatToken:
        addInstruction(WasmInstructionWithData(WasmInstruction::F32Const, std::string(node.token.value)));
        break;
    case LexerTokenType::StringToken:
        // Handle string constants (this might need more complex handling)
        addInstruction(WasmInstructionWithData(WasmInstruction::I32Const, "0")); // Placeholder
        break;
    case LexerTokenType::BoolToken:
        addInstruction(
            WasmInstructionWithData(WasmInstruction::I32Const, node.token.value == "true" ? "true" : "false"));
        break;
    case LexerTokenType::VarToken:
    {
        int localIndex = getOrCreateLocalIndex(node.token.value);
        addInstruction(WasmInstructionWithData(WasmInstruction::LocalGet, std::to_string(localIndex)));
    }
    break;
    case LexerTokenType::EqualToken:
        addInstruction(WasmInstructionWithData(isFloatOperation ? WasmInstruction::F32Eq : WasmInstruction::I32Eq));
        break;
    case LexerTokenType::NotEqualToken:
        addInstruction(WasmInstructionWithData(isFloatOperation ? WasmInstruction::F32Ne : WasmInstruction::I32Ne));
        break;
    case LexerTokenType::GreaterEqualToken:
        addInstruction(WasmInstructionWithData(isFloatOperation ? WasmInstruction::F32Ge : WasmInstruction::I32GeS));
        break;
    case LexerTokenType::GreaterToken:
        addInstruction(WasmInstructionWithData(isFloatOperation ? WasmInstruction::F32Gt : WasmInstruction::I32GtS));
        break;
    case LexerTokenType::LessEqualToken:
        addInstruction(WasmInstructionWithData(isFloatOperation ? WasmInstruction::F32Le : WasmInstruction::I32LeS));
        break;
    case LexerTokenType::LessToken:
        addInstruction(WasmInstructionWithData(isFloatOperation ? WasmInstruction::F32Lt : WasmInstruction::I32LtS));
        break;
    default:
        // Handle unsupported operations
        break;
    }
}

int WasmGen::getOrCreateLocalIndex(std::string_view varName)
{
    auto it = m_locals.find(std::string(varName));
    if (it != m_locals.end())
    {
        return it->second;
    }
    int newIndex                   = m_nextLocalIndex++;
    m_locals[std::string(varName)] = newIndex;
    return newIndex;
}

void WasmGen::addInstruction(WasmInstructionWithData instruction)
{
    m_instructions.push_back(instruction);
}

const std::unordered_map<std::string, int>& WasmGen::getLocalMap() const
{
    return m_locals;
}

bool WasmGen::isFloatType(const BinaryNode& node) const
{
    if (node.left)
        return ScopedSymbolTable::getInstance().isFloatType(node.left->token.value);
    if (node.right)
        return ScopedSymbolTable::getInstance().isFloatType(node.right->token.value);
    return false;
}

void WasmGen::generateConditional(const ConditionalNode& node)
{
    // Generate code for the condition
    generateExpression(static_cast<const BinaryNode&>(*node.condition));

    // Start of if block
    addInstruction(WasmInstructionWithData(WasmInstruction::If));

    // Generate code for the if block
    generateBlock(*node.ifNode);

    // Check if there's an else block
    if (node.elseNode)
    {
        addInstruction(WasmInstructionWithData(WasmInstruction::Else));
        generateBlock(*node.elseNode);
    }

    // End of if-else block
    addInstruction(WasmInstructionWithData(WasmInstruction::End));
}

void WasmGen::generateBlock(const TreeNode& node)
{
    auto blah = node.token;
    for (const auto& block : node.children)
    {
        generate(*block);
    }
    if (blah.type == LexerTokenType::PrintToken)
    {
        addInstruction(WasmInstructionWithData(WasmInstruction::CallPrint));
    }
}

void WasmGen::addGeneratedCodeToOutput()
{

    nlohmann::json instructionArray = nlohmann::json::array();
    for (const auto& instr : m_instructions)
    {
        instructionArray.push_back(instructionToString(instr));
    }
    m_output.getJson()["Gen"].push_back(instructionArray);
}