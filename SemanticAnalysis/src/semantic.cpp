#include "semantic.hpp"
#include "Utils/Error.hpp"


void Semantic::traverse(std::vector<std::shared_ptr<Node>> compound) {
    std::shared_ptr<Node> &node = compound[0];

    if(node == nullptr) { return; }

    if(node->type.type == LexerTokenType::AssignToken) { checkAssignments(node); }
}

void Semantic::checkAssignments (std::shared_ptr<Node> node) {
    // go left and check if left is var
    if(node->left->type.type != LexerTokenType::VarToken) { throw Error("Invalid Assignment ");}
    // check if left has been defined if not insert 
    if (symboltable.search(node->left->type)) { throw Error("Duplicate variable declaration");}
    symboltable.insert(node->left->type);
    // go right 
}


void Semantic::checkExpr(std::shared_ptr<Node> node) {
    
}

