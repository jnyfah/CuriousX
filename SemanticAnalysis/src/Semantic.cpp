#include "Semantic.hpp"
#include "Utils/Error.hpp"


void Semantic::traverse(std::vector<std::shared_ptr<Node>> compound)
{
    for (auto node : compound)
    {
        // std::shared_ptr<Node> node = compound[0];
        if (node == nullptr) { return; }
        if (node->type.type == LexerTokenType::AssignToken)
        {
            checkAssignments(node);
            auto inferredType = inferType(node->right);

            if (!inferredType.has_value()) { throw Error("Variable not defined ", node->right->type.location); }

            node->right->inferType = inferredType.value();
            symboltable.setInferredType(node->right->type.value, inferredType.value());
        }
    }
}


void Semantic::checkAssignments(std::shared_ptr<Node> node)
{
    // go left and check if left is var
    if (node->left->type.type != LexerTokenType::VarToken)
    {
        throw Error("Invalid Assignment ", node->left->type.location);
    }
    // check if left has been defined if not insert
    if (symboltable.search(node->left->type))
    {
        throw Error("Duplicate variable declaration at", node->left->type.location);
    } else
    {
        symboltable.insert(node->left->type, "");
    }

    // Go right
    checkExpr(node->right);
}


void Semantic::checkExpr(std::shared_ptr<Node> node)
{
    // Check Division by Zero
    if (node->type.type == LexerTokenType::DivideToken)
    {
        if ((node->right->type.type == LexerTokenType::IntToken || node->right->type.type == LexerTokenType::FloatToken)
            && node->right->type.value == "0")
        {
            throw Error("Division by zero at", node->right->type.location);
        }
    }
    if (node->left != nullptr)
    {
        if (node->left->type.type == LexerTokenType::VarToken)
        {
            if (!symboltable.search(node->left->type))
            {
                throw Error("Variable not defined ", node->left->type.location);
            }
        }
        checkExpr(node->left);
    }

    if (node->right != nullptr)
    {
        if (node->right->type.type == LexerTokenType::VarToken)
        {
            if (!symboltable.search(node->right->type))
            {
                throw Error("Variable not defined ", node->right->type.location);
            }
        }
        checkExpr(node->right);
    }
}


std::optional<std::string> Semantic::inferType(std::shared_ptr<Node> node)
{
    if (node->left != nullptr) { inferType(node->left); }
    if (node->right != nullptr) { inferType(node->right); }

    // check the type of the current node
    if (node->type.type == LexerTokenType::IntToken)
    {
        return "integer";

    } else if (node->type.type == LexerTokenType::FloatToken)
    {
        return "float";

    } else if (node->type.type == LexerTokenType::VarToken) {

        auto inferredType = symboltable.getInferredType(node->type.value);

        if (!inferredType) { throw Error("Variable not defined ", node->type.location); }

        return inferredType.value();

    } else if ((node->type.type == LexerTokenType::PlusToken) || (node->type.type == LexerTokenType::MinusToken)
               || (node->type.type == LexerTokenType::DivideToken)
               || (node->type.type == LexerTokenType::MultiplyToken)) {

        auto left = inferType(node->left);
        auto right = inferType(node->right);

        std::cout<< left.value()<<std::endl;
        std::cout<< right.value()<<std::endl;

        if (left && right)
        {
            if (left.value() != right.value()) { throw Error("Type mismatch:  at", node->type.location); }
            return left;
        }
    } else
    {

        throw Error("Unbalanced Expression, Missing or extra tokens at", node->type.location);
    }
    return std::nullopt;
}
