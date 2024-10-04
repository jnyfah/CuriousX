#include "Semantic.hpp"
#include "Error.hpp"
#include <cmath>
#include <iostream>
#include <limits>

// if can start a program
// correct parser readme
// fix includes too much circular
// missing closing quotes for string ??
// having just literal expressions starting a program or even anywhere
// performing binary operations on strings not allowed (done)
// division by zero
// scope variables defined in the then block

bool Semantic::analyze(const ASTNode& node)
{

    switch (node.getType())
    {
    case NodeType::BinaryOperation:
        analyzeBinaryOperation(static_cast<const BinaryNode&>(node));
        break;
    case NodeType::ConditionalOperation:
        //analyzeConditionalOperation(static_cast<const ConditionalNode&>(node));
        break;
    case NodeType::PrintProgram:
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
    else
    {
        analyzeExpression(node);
    }
}

void Semantic::analyzeAssignment(const BinaryNode& node)
{
    if (node.left->token.type != LexerTokenType::VarToken)
    {
        throw Error("Invalid assignment: left side must be a variable ", node.left->token.location);
    }
    const std::string& varName = std::string(node.left->token.value);
    InferredType rightType = inferType(*node.right);

    if (m_symboltable.contains(varName))
    {
        auto existingType = m_symboltable.getType(varName);
        if (existingType != rightType)
        {
            throw Error("Type mismatch in assignment ", node.left->token.location);
        }
    }
    else
    {
        m_symboltable.insert(varName, rightType, node.left->token);
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
        return inferTypeFromCondition(static_cast<const BinaryNode&>(node));
    //case LexerTokenType::AssignToken:


    default:
        throw Error("Unable to infer type", node.token.location);
    }
}

InferredType Semantic::inferTypeFromVariable(const ASTNode& node)
{
    auto type = m_symboltable.getType(std::string(node.token.value));
    if (!type)
    {
        throw Error("Variable not defined", node.token.location);
    }
    flag = true;

    return *type;
}

InferredType Semantic::inferTypeFromOperation(const BinaryNode& node)
{
    if (!node.left || !node.right)
    {
        throw Error("Unbalanced expression, missing operand", node.token.location);
    }
    // // check division by zero
    // if (node.token.type == LexerTokenType::DivideToken) checkDivisionByZero(*node.right);

    InferredType leftType = inferType(*node.left);
    InferredType rightType = inferType(*node.right);

    if (leftType != rightType)
    {
        throw Error("Type mismatch in operation at ", node.token.location);
    }

    if (leftType == InferredType::STRING || rightType == InferredType::STRING)
    {
        throw Error("no Binary Operations on Strings ", node.token.location);
    }

    return leftType;
}

InferredType Semantic::inferTypeFromCondition(const BinaryNode& node)
{
    if (!node.left || !node.right)
    {
        throw Error("Unbalanced expression, missing operand", node.token.location);
    }
    InferredType leftType = inferType(*node.left);
    InferredType rightType = inferType(*node.right);
    if (leftType != rightType)
    {
        throw Error("Type mismatch in operation at ", node.token.location);
    }

    return leftType;
}

void Semantic::analyzeExpression(const BinaryNode& node)
{
    InferredType rightType = inferType(*node.right);
    InferredType leftType = inferType(*node.left);
    if (rightType != leftType)
    {
        throw Error("Type mismatch in operation at ", node.token.location);
    }
    if (!flag)
        throw Error("no variable", node.token.location);
}

void Semantic::analyzeConditionalOperation(const ConditionalNode& node)
{
    inferType(*node.condition);
     inferType(*node.ifNode);
     if (node.elseNode)
         inferType(*node.elseNode);
}

