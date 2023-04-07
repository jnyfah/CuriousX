#include "../include/Codegen.hpp"



std::string CodeGen::preamble() {
    return ".syntax unified\n"
           ".cpu cortex-m0\n"  // Specifies the target processor architecture
           ".thumb\n"          // Indicates that the code will be in Thumb mode
           ".global main\n"    // Marks the main function as global
           "    main:\n\n";          // Marks the main function as global
}

std::string CodeGen::convert(const std::vector<std::shared_ptr<Node>>& compound) {
    // Add the global label at the beginning
    output_ += preamble();

    // Traverse the AST in postfix order
    for (auto& node : compound) {
        traverse(node);
        output_ += "\n";

    }

    // Add a return statement to the end of the main function
    output_ += "\t BX lr\n";       // Return control to the caller

    return output_;
}

void CodeGen::traverse(const std::shared_ptr<Node> node) {
    int reg_idx;

    switch (node->type.type) {
        case LexerTokenType::AssignToken:
            traverse(node->right);
            reg_idx = last_reg;
            traverse(node->left);
            if (_symboltable.getInferredType(node->left->type.value) == InferredType::FLOAT) {
                output_ += "\t VSTR s" + std::to_string(reg_idx) + ", [s" + std::to_string(last_reg) + "]\n";
            }else {
                output_ += "\t str r" + std::to_string(reg_idx) + ", [r" + std::to_string(last_reg) + "]\n";
            }
            reg.free_register(last_reg);
            reg.free_register(reg_idx);
            break;

        case LexerTokenType::IntToken:
            reg_idx = reg.alloc_register();
            output_ += "\t MOV r" + std::to_string(reg_idx) + ", #" + node->type.value + "\n";
            last_reg = reg_idx;
            break;

        case LexerTokenType::FloatToken:
            reg_idx = reg.alloc_register();
            output_ += "\t VLDR s" + std::to_string(reg_idx) + ", =" + node->type.value + "\n";
            last_reg = reg_idx;
            break;

        case LexerTokenType::VarToken:
            reg_idx = reg.alloc_register();
            if (_symboltable.getInferredType(node->type.value) == InferredType::FLOAT) {
                output_ += "\t VLDR s" + std::to_string(reg_idx) + ", =" + node->type.value + "\n";
            }else {
                output_ += "\t LDR r" + std::to_string(reg_idx) + ", =" + node->type.value + "\n";
            }
            last_reg = reg_idx;
            break;

        case LexerTokenType::PlusToken:
            traverse(node->left);
            reg_idx = last_reg;
            traverse(node->right);
            if (node->left->type.type == LexerTokenType::FloatToken || _symboltable.getInferredType(node->left->type.value) == InferredType::FLOAT) {
                output_ += "\t VADD.F32, s" + std::to_string(reg_idx) + ", s" + std::to_string(last_reg) + "\n";
            } else {
                output_ += "\t ADD r" + std::to_string(reg_idx) + ", r" + std::to_string(reg_idx) + ", r" + std::to_string(last_reg) + "\n";
            }
            reg.free_register(last_reg);
            last_reg = reg_idx;
            break;

        case LexerTokenType::MultiplyToken:
            traverse(node->left);
            reg_idx = last_reg;
            traverse(node->right);
            if (node->left->type.type == LexerTokenType::FloatToken || _symboltable.getInferredType(node->left->type.value) == InferredType::FLOAT) {
                output_ += "\t VMUL.F32, s" + std::to_string(reg_idx) + ", s" + std::to_string(last_reg) + "\n";
            } else {
                output_ += "\t MUL r" + std::to_string(reg_idx) + ", r" + std::to_string(reg_idx) + ", r" + std::to_string(last_reg) + "\n";
            }
            reg.free_register(last_reg);
            last_reg = reg_idx;
            break;
        
        case LexerTokenType::MinusToken:
            traverse(node->left);
            reg_idx = last_reg;
            traverse(node->right);
            if (node->left->type.type == LexerTokenType::FloatToken || _symboltable.getInferredType(node->left->type.value) == InferredType::FLOAT) {
                output_ += "\t VDIV.F32, s" + std::to_string(reg_idx) + ", s" + std::to_string(last_reg) + "\n";
            } else {
                output_ += "\t DIV r" + std::to_string(reg_idx) + ", r" + std::to_string(reg_idx) + ", r" + std::to_string(last_reg) + "\n";
            }
            reg.free_register(last_reg);
            last_reg = reg_idx;
            break;

        case LexerTokenType::DivideToken:
            traverse(node->left);
            reg_idx = last_reg;
            traverse(node->right);
            if (node->left->type.type == LexerTokenType::FloatToken || _symboltable.getInferredType(node->left->type.value) == InferredType::FLOAT) {
                output_ += "\t VDIV.F32, s" + std::to_string(reg_idx) + ", s" + std::to_string(last_reg) + "\n";
            } else {
                output_ += "\t DIV r" + std::to_string(reg_idx) + ", r" + std::to_string(reg_idx) + ", r" + std::to_string(last_reg) + "\n";
            }
            reg.free_register(last_reg);
            last_reg = reg_idx;
            break;


    case LexerTokenType::PrintToken:
        output_ += "print: \n";
        traverse(node->left);
        int reg_idx = reg.alloc_register();
        output_ += "\t BL printf\n";

        reg.free_register(reg_idx);
}

}

