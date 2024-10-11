#include "Semantic.hpp"
#include <cmath>
#include <limits>

// if can start a program ?
// division by zero

bool Semantic::analyze(const ASTNode& node)
{
    switch (node.getType())
    {
    case NodeType::BinaryOperation:
        analyzeBinaryOperation(static_cast<const BinaryNode&>(node));
        break;
    case NodeType::ConditionalOperation:
        analyzeConditionalOperation(static_cast<const ConditionalNode&>(node));
        break;
    case NodeType::BlockOperation:
        analyzeBlockOperation(static_cast<const TreeNode&>(node));
        break;
    default:
        break;
    }
    return true;
}

void Semantic::analyzeBinaryOperation(const BinaryNode& node)
{
    flag = false;
    if (node.token.type == LexerTokenType::AssignToken)
    {
        analyzeAssignment(node);
    }
    else if (node.token.type == LexerTokenType::VarToken)
    {
        inferType(node);
    }
    else
    {
        analyzeExpression(node);
    }
}

void Semantic::analyzeAssignment(const BinaryNode& node)
{
    if (node.left->token.type != LexerTokenType::VarToken)
    {
        throw Error("Invalid assignment: left side must be a variable", node.left->token.location);
    }
    const std::string& varName = std::string(node.left->token.value);
    InferredType rightType = inferType(*node.right);

    if (ScopedSymbolTable::getInstance().contains(varName))
    {
        auto existingType = ScopedSymbolTable::getInstance().lookup(varName);
        if (existingType != rightType)
        {
            throw Error("Type mismatch in assignment", node.left->token.location);
        }
    }
    else
    {
        ScopedSymbolTable::getInstance().insert(varName, rightType, node.left->token);
    }
}

InferredType Semantic::inferType(const ASTNode& node)
{
    switch (node.token.type)
    {
    case LexerTokenType::IntToken:
        return InferredType::INTEGER;
    case LexerTokenType::FloatToken:
        return InferredType::FLOAT;
    case LexerTokenType::StringToken:
        return InferredType::STRING;
    case LexerTokenType::BoolToken:
        return InferredType::BOOL;
    case LexerTokenType::VarToken:
        return inferTypeFromVariable(node);
    case LexerTokenType::PlusToken:
    case LexerTokenType::MinusToken:
    case LexerTokenType::DivideToken:
    case LexerTokenType::MultiplyToken:
        return inferTypeFromOperation(static_cast<const BinaryNode&>(node));
    case LexerTokenType::EqualToken:
    case LexerTokenType::NotEqualToken:
    case LexerTokenType::GreaterEqualToken:
    case LexerTokenType::GreaterToken:
    case LexerTokenType::LessEqualToken:
    case LexerTokenType::LessToken:
        return inferTypeFromOperation(static_cast<const BinaryNode&>(node));
    default:
        throw Error("Unable to infer type", node.token.location, ErrorType::SEMANTIC);
    }
}

InferredType Semantic::inferTypeFromVariable(const ASTNode& node)
{
    auto type = ScopedSymbolTable::getInstance().lookup(std::string(node.token.value));
    if (!type)
    {
        throw Error("Variable not defined", node.token.location, ErrorType::SEMANTIC);
    }
    flag = true;

    return *type;
}

InferredType Semantic::inferTypeFromOperation(const BinaryNode& node)
{
    if (!node.left || !node.right)
    {
        throw Error("Unbalanced expression, missing operand", node.token.location,
                    ErrorType::SEMANTIC);
    }
    // // check division by zero
    // if (node.token.type == LexerTokenType::DivideToken) checkDivisionByZero(*node.right);

    InferredType leftType = inferType(*node.left);
    InferredType rightType = inferType(*node.right);

    if (leftType != rightType)
    {
        throw Error("Type mismatch in operation", node.token.location, ErrorType::SEMANTIC);
    }
    return leftType;
}

void Semantic::analyzeExpression(const BinaryNode& node)
{
    InferredType rightType = inferType(*node.right);
    InferredType leftType = inferType(*node.left);
    if (rightType != leftType)
    {
        throw Error("Type mismatch in operation", node.token.location, ErrorType::SEMANTIC);
    }
    if (!flag)
        throw Error("literal Expressions not allowed", node.token.location, ErrorType::SEMANTIC);
}

void Semantic::analyzeConditionalOperation(const ConditionalNode& node)
{
    if (!isValidConditionType(node.condition->token))
    {
        throw Error("Expected a boolean expression", node.condition->token.location);
    }
    inferType(*node.condition);
    analyzeBlockOperation(*node.ifNode);
    if (node.elseNode)
        analyzeBlockOperation(*node.elseNode);
}

bool Semantic::isValidConditionType(const LexerToken& token)
{
    return (token.type == LexerTokenType::EqualToken ||
            token.type == LexerTokenType::GreaterEqualToken ||
            token.type == LexerTokenType::GreaterToken ||
            token.type == LexerTokenType::LessEqualToken ||
            token.type == LexerTokenType::LessToken || token.type == LexerTokenType::NotEqualToken);
}

bool Semantic::isValidBinaryType(const LexerToken& token)
{
    return (token.type == LexerTokenType::PlusToken || token.type == LexerTokenType::MinusToken ||
            token.type == LexerTokenType::MultiplyToken ||
            token.type == LexerTokenType::DivideToken);
}

void Semantic::analyzeBlockOperation(const TreeNode& node)
{
    ScopedSymbolTable::getInstance().enterScope();

    for (const auto& statement : node.children)
    {
        analyze(*statement);
    }
    ScopedSymbolTable::getInstance().exitScope();
}

const std::vector<std::unordered_map<std::string, SymbolInfo>> Semantic::getSymbolTable()
{
    return ScopedSymbolTable::getInstance().getSymbolTable();
}