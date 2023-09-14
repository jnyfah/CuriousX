#include "../include/Codegen.hpp"

nlohmann::json CodeGen::convert(const std::vector<std::shared_ptr<Node>> &compound)
{
    nlohmann::json jArray = nlohmann::json::array();
    // Traverse the AST in postfix order
    for (auto &node : compound) { jArray.push_back(traverse(node)); }

    return jArray;
}



nlohmann::json CodeGen::traverse(const std::shared_ptr<Node> node)
{
    int reg_idx;

    nlohmann::json instructions = nlohmann::json::array();
    nlohmann::json instruction;

    switch (node->type.type)
    {
    case LexerTokenType::AssignToken: {
        instruction.clear();
        nlohmann::json rightInstructions = traverse(node->right);
        instructions.insert(instructions.end(), rightInstructions.begin(), rightInstructions.end());

        reg_idx = last_reg;

        nlohmann::json leftInstructions = traverse(node->left);
        instructions.insert(instructions.end(), leftInstructions.begin(), leftInstructions.end());

        if (_symboltable.getInferredType(node->left->type.value) == InferredType::FLOAT)
        {
            instruction["command"] = "vstr";
            instruction["register"] = "s" + std::to_string(reg_idx);
            instruction["value"] = "[sp, #-4]!";
        } else
        {
            instruction["command"] = "str";
            instruction["register"] = "r" + std::to_string(reg_idx);
            instruction["value"] = "[sp, #-4]!";
        }
        instructions.push_back(instruction);
        reg.free_register(last_reg);
        reg.free_register(reg_idx);
        break;
    }

    case LexerTokenType::IntToken:
        instruction.clear();
        reg_idx = reg.alloc_register();
        instruction["command"] = "mov";
        instruction["register"] = "r" + std::to_string(reg_idx);
        instruction["value"] = "#" + node->type.value;
        instructions.push_back(instruction);
        last_reg = reg_idx;
        break;

    case LexerTokenType::FloatToken:
        instruction.clear();
        reg_idx = reg.alloc_register();
        instruction["command"] = "vmov";
        instruction["register"] = "s" + std::to_string(reg_idx);
        instruction["value"] = "#" + node->type.value;
        instructions.push_back(instruction);
        last_reg = reg_idx;
        break;

    case LexerTokenType::VarToken:
        instruction.clear();
        reg_idx = reg.alloc_register();
        if (_staked_var.find(node->type.value) != _staked_var.end())
        {
            if (_symboltable.getInferredType(node->type.value) == InferredType::FLOAT)
            {
                instruction["command"] = "vldr";
                instruction["register"] = "s" + std::to_string(reg_idx);
                instruction["value"] = "[sp, #" + std::to_string(_staked_var[node->type.value]) + "]";
            } else
            {
                instruction["command"] = "ldr";
                instruction["register"] = "r" + std::to_string(reg_idx);
                instruction["value"] = "[sp, #" + std::to_string(_staked_var[node->type.value]) + "]";
            }
        } else
        {
            _staked_var[node->type.value] = (sp += 4);
        }
        instructions.push_back(instruction);
        last_reg = reg_idx;
        break;

    case LexerTokenType::PlusToken:
    case LexerTokenType::MultiplyToken: {
        instruction.clear();

        nlohmann::json leftInstructions = traverse(node->left);
        instructions.insert(instructions.end(), leftInstructions.begin(), leftInstructions.end());

        reg_idx = last_reg;
        nlohmann::json rightInstructions = traverse(node->right);
        instructions.insert(instructions.end(), rightInstructions.begin(), rightInstructions.end());
        if (node->left->type.type == LexerTokenType::FloatToken
            || _symboltable.getInferredType(node->left->type.value) == InferredType::FLOAT)
        {
            if (node->type.type == LexerTokenType::PlusToken)
            {
                instruction["command"] = "add.F32";
                instruction["registers"] = { "s" + std::to_string(reg_idx), "s" + std::to_string(last_reg) };
            } else
            {
                instruction["command"] = "mul.f32";
                instruction["registers"] = { "s" + std::to_string(reg_idx), "s" + std::to_string(last_reg) };
            }

        } else
        {
            if (node->type.type == LexerTokenType::PlusToken)
            {
                instruction["command"] = "add";
                instruction["registers"] = {
                    "r" + std::to_string(reg_idx), "r" + std::to_string(reg_idx), "r" + std::to_string(last_reg)
                };
            } else
            {
                instruction["command"] = "mul";
                instruction["registers"] = {
                    "r" + std::to_string(reg_idx), "r" + std::to_string(reg_idx), "r" + std::to_string(last_reg)
                };
            }
        }

        instructions.push_back(instruction);
        reg.free_register(last_reg);
        last_reg = reg_idx;
        break;
    }

    case LexerTokenType::MinusToken:
    case LexerTokenType::DivideToken: {
        instruction.clear();
        nlohmann::json leftInstructions = traverse(node->left);
        instructions.insert(instructions.end(), leftInstructions.begin(), leftInstructions.end());

        reg_idx = last_reg;
        nlohmann::json rightInstructions = traverse(node->left);
        instructions.insert(instructions.end(), rightInstructions.begin(), rightInstructions.end());

        if (node->left->type.type == LexerTokenType::FloatToken
            || _symboltable.getInferredType(node->left->type.value) == InferredType::FLOAT)
        {
            if (node->type.type == LexerTokenType::MinusToken)
            {
                instruction["command"] = "vsub.f32";
                instruction["registers"] = { "s" + std::to_string(reg_idx), "s" + std::to_string(last_reg) };
            } else
            {
                instruction["command"] = "vdiv.f32";
                instruction["registers"] = { "s" + std::to_string(reg_idx), "s" + std::to_string(last_reg) };
            }

        } else
        {
            if (node->type.type == LexerTokenType::PlusToken)
            {
                instruction["command"] = "sub";
                instruction["registers"] = {
                    "r" + std::to_string(reg_idx), "r" + std::to_string(reg_idx), "r" + std::to_string(last_reg)
                };
            } else
            {
                instruction["command"] = "div";
                instruction["registers"] = {
                    "r" + std::to_string(reg_idx), "r" + std::to_string(reg_idx), "r" + std::to_string(last_reg)
                };
            }
        }
        instructions.push_back(instruction);
        reg.free_register(last_reg);
        last_reg = reg_idx;
        break;
    }

    case LexerTokenType::PrintToken:
        instruction.clear();
        instruction["command"] = "print";
        instruction["registers"] = { "" };
        instructions.push_back(instruction);
        traverse(node->left);
        nlohmann::json leftInstructions = traverse(node->left);
        instructions.insert(instructions.end(), leftInstructions.begin(), leftInstructions.end());
        int reg_idx = reg.alloc_register();
        instruction["command"] = "bl";
        instruction["registers"] = { "" };
        instruction["value"] = "printf";
        instructions.push_back(instruction);

        reg.free_register(reg_idx);
    }
    return instructions;
}
