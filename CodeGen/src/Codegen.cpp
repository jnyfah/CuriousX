#include "../include/Codegen.hpp"


void CodeGen::convert(const std::vector<std::shared_ptr<Node>>& compound) {

    // Traverse the AST in postfix order
    for (auto& node : compound) {
        traverse(node);
        output_stream << "\n";
    }

    // Add a return statement to the end of the main function
    output_stream << "\t bx lr";
}

void CodeGen::traverse(const std::shared_ptr<Node> node) {
    int reg_idx;

    switch (node->type.type) {
        case LexerTokenType::AssignToken:
            traverse(node->right);
            reg_idx = last_reg;
            traverse(node->left);
            if (_symboltable.getInferredType(node->left->type.value) == InferredType::FLOAT) {
                output_stream << "\t vstr s" << reg_idx << ", [sp, #-4]!\n";
            }else {
                output_stream << "\t str r" << reg_idx << ", [sp, #-4]!\n";
            }
            reg.free_register(last_reg);
            reg.free_register(reg_idx);
            break;

        case LexerTokenType::IntToken:
            reg_idx = reg.alloc_register();
            output_stream << "\t mov r" << reg_idx << ", #" << node->type.value << "\n";
            last_reg = reg_idx;
            break;

        case LexerTokenType::FloatToken:
            reg_idx = reg.alloc_register();
            output_stream << "\t vldr s" << reg_idx << ", #" << node->type.value << "\n";
            last_reg = reg_idx;
            break;

        case LexerTokenType::VarToken:
            reg_idx = reg.alloc_register();
            if(_staked_var.find(node->type.value) != _staked_var.end()){
                if (_symboltable.getInferredType(node->type.value) == InferredType::FLOAT) {
                    output_stream << "\t vldr s" << reg_idx << ", [sp, #" << std::to_string(_staked_var[node->type.value])<< "]\n";
                }else {
                    output_stream << "\t ldr r" << reg_idx << ", [sp, #" << std::to_string(_staked_var[node->type.value])<< "]\n";
                }
            }else {
                _staked_var[node->type.value] = (sp +=4);
            }
            
            last_reg = reg_idx;
            break;

        case LexerTokenType::PlusToken:
            traverse(node->left);
            reg_idx = last_reg;
            traverse(node->right);
            if (node->left->type.type == LexerTokenType::FloatToken || _symboltable.getInferredType(node->left->type.value) == InferredType::FLOAT) {
                output_stream << "\t add.F32, s" << reg_idx << ", s" << last_reg << "\n";
            } else {
                output_stream << "\t add r" << reg_idx << ", r" << reg_idx << ", r" << last_reg << "\n";
            }
            reg.free_register(last_reg);
            last_reg = reg_idx;
            break;

        case LexerTokenType::MultiplyToken:
            traverse(node->left);
            reg_idx = last_reg;
            traverse(node->right);
            if (node->left->type.type == LexerTokenType::FloatToken || _symboltable.getInferredType(node->left->type.value) == InferredType::FLOAT) {
                output_stream << "\t mul.f32, s" << reg_idx << ", s" << last_reg << "\n";
            } else {
                output_stream << "\t mul r" << reg_idx << ", r" << reg_idx << ", r" << last_reg << "\n";
            }
            reg.free_register(last_reg);
            last_reg = reg_idx;
            break;
        
        case LexerTokenType::MinusToken:
            traverse(node->left);
            reg_idx = last_reg;
            traverse(node->right);
            if (node->left->type.type == LexerTokenType::FloatToken || _symboltable.getInferredType(node->left->type.value) == InferredType::FLOAT) {
                output_stream << "\t vsub.f32, s" << reg_idx << ", s" << last_reg << "\n";
            } else {
                output_stream << "\t sub r" << reg_idx << ", r" << reg_idx << ", r" << last_reg << "\n";
            }
            reg.free_register(last_reg);
            last_reg = reg_idx;
            break;

        case LexerTokenType::DivideToken:
            traverse(node->left);
            reg_idx = last_reg;
            traverse(node->right);
            if (node->left->type.type == LexerTokenType::FloatToken || _symboltable.getInferredType(node->left->type.value) == InferredType::FLOAT) {
                output_stream << "\t vdiv.f32, s" << reg_idx << ", s" << last_reg << "\n";
            } else {
                output_stream << "\t div r" << reg_idx << ", r" << reg_idx << ", r" << last_reg << "\n";
            }
            reg.free_register(last_reg);
            last_reg = reg_idx;
            break;


    case LexerTokenType::PrintToken:
        output_stream << "print: \n";
        traverse(node->left);
        int reg_idx = reg.alloc_register();
        output_stream << "\t bl printf\n";

        reg.free_register(reg_idx);
}

}

