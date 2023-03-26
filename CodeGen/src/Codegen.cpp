#include "../include/Codegen.hpp"



std::string CodeGen::preamble() {
    return ".syntax unified\n"
           ".cpu cortex-m0\n"  // Specifies the target processor architecture
           ".thumb\n"          // Indicates that the code will be in Thumb mode
           ".global main\n";   // Marks the main function as global
}

std::string CodeGen::convert(const std::vector<std::shared_ptr<Node>> &compound) {
    // Add the global label at the beginning
    output_ += preamble();

    // Traverse the AST in postfix order
    for (auto& node : compound) {
        traverse(node);
    }

    return output_;
}

void CodeGen::traverse(const std::shared_ptr<Node> node) {
    int reg_idx;

    switch (node->type.type) {
        case LexerTokenType::AssignToken:
            traverse(node->right);
            reg_idx = reg.alloc_register();
            output_ += "str r" + std::to_string(last_reg) + ", [sp, #-4]!\n";
            traverse(node->left);
            output_ += "ldr r" + std::to_string(reg_idx) + ", [sp], #4\n"; // Use the same register index here
            output_ += "str r" + std::to_string(reg_idx) + ", [r" + std::to_string(reg_idx) + "]\n";
            reg.free_register(reg_idx);
            break;
        case LexerTokenType::IntToken:
            reg_idx = reg.alloc_register();
            output_ += "mov r" + std::to_string(reg_idx) + ", #" + node->type.value + "\n";
            last_reg = reg_idx;
            break;
        default:
            break;
    }
}


