#include "Semantic.hpp"
#include "Utils/Error.hpp"


void Semantic::traverse(const std::vector<std::shared_ptr<Node>> &compound)
{
    for (auto node : compound)
    {
        if (node == nullptr) { return; }

        if (node->type.type == LexerTokenType::AssignToken)
        {
            auto infertype = inferType(node->right).value();
            checkAssignments(node, infertype);

        } else if (node->type.type == LexerTokenType::PrintToken)
        {
            // make sure variables in print are already defined and no equal signs allowed
            checkPrint(node);
        } else
        {
            throw Error("Invalid Program structure ", node->type.location);
        }
    }
}


void Semantic::checkAssignments(std::shared_ptr<Node> node, const InferredType &inferredType)
{
    // go left and check if left is var
    if (node->left->type.type != LexerTokenType::VarToken)
    {
        throw Error("Invalid Assignment ", node->left->type.location);
    }
    // check if left has been defined if not insert
    if (symboltable.search(node->left->type))
    {
        // mutable
        // 1. check if type is same
        if (inferredType != symboltable.getInferredType(node->left->type.value).value())
        {
            throw Error("Duplicate variable declaration at", node->left->type.location);
        }

    } else
    {
        symboltable.insert(node->left->type, inferredType);
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





std::optional<InferredType> Semantic::inferType(std::shared_ptr<Node> node)
{
    // check the type of the current node
    if (node->type.type == LexerTokenType::IntToken)
    {
        return InferredType::INTEGER;

    } else if (node->type.type == LexerTokenType::FloatToken)
    {
        return InferredType::FLOAT;

    } else if (node->type.type == LexerTokenType::VarToken)
    {

        auto inferredType = symboltable.getInferredType(node->type.value);

        if (!inferredType) { throw Error("Variable not defined ", node->type.location); }

        return inferredType.value();

    } else if ((node->type.type == LexerTokenType::PlusToken) || (node->type.type == LexerTokenType::MinusToken)
               || (node->type.type == LexerTokenType::DivideToken)
               || (node->type.type == LexerTokenType::MultiplyToken))
    {

        if (!(node->left && node->right))
        {
            throw Error("Unbalanced Expression, Missing or extra tokens at", node->type.location);
        }

        auto left = inferType(node->left);
        auto right = inferType(node->right);

        if (left.value() != right.value()) { throw Error("Type mismatch:  at", node->type.location); }

        return left.value();

    } else
    {

        throw Error("Unbalanced Expression, Missing or extra tokens at", node->type.location);
    }
    return std::nullopt;
}


void Semantic::checkPrint(std::shared_ptr<Node> node)
{
    if (node->left->type.type == LexerTokenType::AssignToken)
    {
        throw Error("Assignments not allowed in print fuctions ", node->left->type.location);
    }
    auto infertype = inferType(node->left);
    if (infertype == std::nullopt) { throw Error("unknown type in Print", node->left->type.location); }
    checkExpr(node->left);
}


nlohmann::json Semantic::printSymbolTree() { return symboltable.tableToJson(); }


symbolTable::Table Semantic::getSymbolTable() const { return symboltable; }
