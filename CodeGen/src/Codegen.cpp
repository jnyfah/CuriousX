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
    }

    // Add a return statement to the end of the main function
    output_ += "\t mov r0, #0\n"  // Set the return value to 0
               "\t bx lr\n";       // Return control to the caller

    return output_;
}

void CodeGen::traverse(const std::shared_ptr<Node> node) {
    int reg_idx;

    switch (node->type.type) {
        case LexerTokenType::AssignToken:
            traverse(node->right);
            //reg_idx = reg.alloc_register();
            output_ += "\t str r" + std::to_string(last_reg) + ", [sp, #-4]!\n";
            traverse(node->left);
            output_ += "\t ldr r" + std::to_string(reg_idx) + ", [sp], #4\n"; // Use the same register index here
            output_ += "\t str r" + std::to_string(reg_idx) + ", [r" + std::to_string(last_reg) + "]\n";
            reg.free_register(reg_idx);
            break;

        case LexerTokenType::IntToken:
            // load immediate 
            reg_idx = reg.alloc_register();
            output_ += "\t mov r" + std::to_string(reg_idx) + ", #" + node->type.value + "\n";
            last_reg = reg_idx;
            break;

        case LexerTokenType::VarToken:
            // Load variable address into a register
            reg_idx = reg.alloc_register();
            output_ += "\t ldr r" + std::to_string(reg_idx) + ", =" + node->type.value + "\n";
            last_reg = reg_idx;
            break;


        case LexerTokenType::PlusToken:
            traverse(node->left);
            reg_idx = last_reg;
            traverse(node->right);
            output_ += "\t add r" + std::to_string(reg_idx) + ", r" + std::to_string(reg_idx) + ", r" + std::to_string(last_reg) + "\n";
            reg.free_register(last_reg);
            last_reg = reg_idx;
            break;

        case LexerTokenType::MinusToken:
            traverse(node->left);
            reg_idx = last_reg;
            traverse(node->right);
            output_ += "\t sub r" + std::to_string(reg_idx) + ", r" + std::to_string(reg_idx) + ", r" + std::to_string(last_reg) + "\n";
            reg.free_register(last_reg);
            last_reg = reg_idx;
            break;

        case LexerTokenType::MultiplyToken:
            traverse(node->left);
            reg_idx = last_reg;
            traverse(node->right);
            output_ += "\t mul r" + std::to_string(reg_idx) + ", r" + std::to_string(reg_idx) + ", r" + std::to_string(last_reg) + "\n";
            reg.free_register(last_reg);
            last_reg = reg_idx;
            break;

        case LexerTokenType::DivideToken:
            traverse(node->left);
            reg_idx = last_reg;
            traverse(node->right);
            output_ += "\t div r" + std::to_string(reg_idx) + ", r" + std::to_string(reg_idx) + ", r" + std::to_string(last_reg) + "\n";
            reg.free_register(last_reg);
            last_reg = reg_idx;
            break;
        default:
            break;
    }
}
