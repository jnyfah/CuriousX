#include "../include/Codegen.hpp"


std::string CodeGen::convert(const std::vector<std::shared_ptr<Node>>& compound) {

    // Traverse the AST in postfix order
    for (auto& node : compound) {
        traverse(node);
        output_ += "\n";
    }

    // Add a return statement to the end of the main function
    output_ += "\t bx lr\n";       // Return control to the caller

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
                output_ += "\t vstr s" + std::to_string(reg_idx) + ", [sp, #-4]!\n";
            }else {
                output_ += "\t str r" + std::to_string(reg_idx) + ", [sp, #-4]!\n";
            }
            reg.free_register(last_reg);
            reg.free_register(reg_idx);
            break;

        case LexerTokenType::IntToken:
            reg_idx = reg.alloc_register();
            output_ += "\t mov r" + std::to_string(reg_idx) + ", #" + node->type.value + "\n";
            last_reg = reg_idx;
            break;

        case LexerTokenType::FloatToken:
            reg_idx = reg.alloc_register();
            output_ += "\t vldr s" + std::to_string(reg_idx) + ", =" + node->type.value + "\n";
            last_reg = reg_idx;
            break;

        case LexerTokenType::VarToken:
            reg_idx = reg.alloc_register();
            if(_staked_var.find(node->type.value) != _staked_var.end()){
                if (_symboltable.getInferredType(node->type.value) == InferredType::FLOAT) {
                    output_ += "\t vldr s" + std::to_string(reg_idx) + ", [sp, #" + std::to_string(_staked_var[node->type.value])+ "]\n";
                }else {
                    output_ += "\t ldr r" + std::to_string(reg_idx) + ", [sp, #" + std::to_string(_staked_var[node->type.value])+ "]\n";
                }
            }else {
                _staked_var[node->type.value]+= (sp+=4);
            }
            
            last_reg = reg_idx;
            break;

        case LexerTokenType::PlusToken:
            traverse(node->left);
            reg_idx = last_reg;
            traverse(node->right);
            if (node->left->type.type == LexerTokenType::FloatToken || _symboltable.getInferredType(node->left->type.value) == InferredType::FLOAT) {
                output_ += "\t add.F32, s" + std::to_string(reg_idx) + ", s" + std::to_string(last_reg) + "\n";
            } else {
                output_ += "\t add r" + std::to_string(reg_idx) + ", r" + std::to_string(reg_idx) + ", r" + std::to_string(last_reg) + "\n";
            }
            reg.free_register(last_reg);
            last_reg = reg_idx;
            break;

        case LexerTokenType::MultiplyToken:
            traverse(node->left);
            reg_idx = last_reg;
            traverse(node->right);
            if (node->left->type.type == LexerTokenType::FloatToken || _symboltable.getInferredType(node->left->type.value) == InferredType::FLOAT) {
                output_ += "\t mul.F32, s" + std::to_string(reg_idx) + ", s" + std::to_string(last_reg) + "\n";
            } else {
                output_ += "\t mul r" + std::to_string(reg_idx) + ", r" + std::to_string(reg_idx) + ", r" + std::to_string(last_reg) + "\n";
            }
            reg.free_register(last_reg);
            last_reg = reg_idx;
            break;
        
        case LexerTokenType::MinusToken:
            traverse(node->left);
            reg_idx = last_reg;
            traverse(node->right);
            if (node->left->type.type == LexerTokenType::FloatToken || _symboltable.getInferredType(node->left->type.value) == InferredType::FLOAT) {
                output_ += "\t vsub.f32, s" + std::to_string(reg_idx) + ", s" + std::to_string(last_reg) + "\n";
            } else {
                output_ += "\t sub r" + std::to_string(reg_idx) + ", r" + std::to_string(reg_idx) + ", r" + std::to_string(last_reg) + "\n";
            }
            reg.free_register(last_reg);
            last_reg = reg_idx;
            break;

        case LexerTokenType::DivideToken:
            traverse(node->left);
            reg_idx = last_reg;
            traverse(node->right);
            if (node->left->type.type == LexerTokenType::FloatToken || _symboltable.getInferredType(node->left->type.value) == InferredType::FLOAT) {
                output_ += "\t vdiv.f32, s" + std::to_string(reg_idx) + ", s" + std::to_string(last_reg) + "\n";
            } else {
                output_ += "\t div r" + std::to_string(reg_idx) + ", r" + std::to_string(reg_idx) + ", r" + std::to_string(last_reg) + "\n";
            }
            reg.free_register(last_reg);
            last_reg = reg_idx;
            break;


    case LexerTokenType::PrintToken:
        output_ += "print: \n";
        traverse(node->left);
        int reg_idx = reg.alloc_register();
        output_ += "\t bL printf\n";

        reg.free_register(reg_idx);
}

}

