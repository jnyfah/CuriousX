#include "Semantic.hpp"
#include <cmath>
#include <limits>
#include <string>

void Semantic::analyzeTree(const ASTNode& node)
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
        throw Error("Unexpected node type", node.token.location, ErrorType::SEMANTIC);
        break;
    }
}

void Semantic::analyzeBinaryOperation(const BinaryNode& node)
{
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

    const std::string& varName   = getVariableName(*node.left);
    InferredType       rightType = inferType(*node.right);

    auto& symbolTable = ScopedSymbolTable::getInstance();

    if (symbolTable.contains(varName))
    {
        auto existingType = symbolTable.lookup(varName);
        ensureTypeMatch(existingType.value(), rightType, node.left->token);
    }
    else
    {
        symbolTable.insert(varName, rightType, node.left->token);
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

void Semantic::analyzeExpression(const BinaryNode& node)
{
    if (node.left && node.right)
    {
        ensureTypeMatch(inferType(*node.left), inferType(*node.right), node.token);
    }

    if (!containsNonLiteral(node))
    {
        throw Error("Literal expressions without effect are not allowed", node.token.location, ErrorType::SEMANTIC);
    }
}

bool Semantic::containsNonLiteral(const ASTNode& node) const
{
    if (node.token.type == LexerTokenType::VarToken) {
        return true;
    }

    if (node.getType() == NodeType::BinaryOperation) {
        const auto* binaryNode = dynamic_cast<const BinaryNode*>(&node);
        
        if (binaryNode) {
            bool leftHasNonLiteral = binaryNode->left && containsNonLiteral(*binaryNode->left);
            bool rightHasNonLiteral = binaryNode->right && containsNonLiteral(*binaryNode->right);
            return leftHasNonLiteral || rightHasNonLiteral;
        }
    }
    return false;
}

void Semantic::ensureTypeMatch(InferredType left, InferredType right, const LexerToken& token) const
{
    if (left != right)
    {
        throw Error("Type mismatch in operation", token.location, ErrorType::SEMANTIC);
    }
}
InferredType Semantic::inferTypeFromVariable(const ASTNode& node)
{
    auto type = ScopedSymbolTable::getInstance().lookup(std::string(node.token.value));
    if (!type)
    {
        throw Error("Variable not defined", node.token.location, ErrorType::SEMANTIC);
    }
    return *type;
}

bool Semantic::isComparisonOp(const BinaryNode& node)
{
    return (node.token.type == LexerTokenType::LessEqualToken || node.token.type == LexerTokenType::LessToken ||
            node.token.type == LexerTokenType::GreaterEqualToken || node.token.type == LexerTokenType::GreaterToken);
}

InferredType Semantic::inferTypeFromOperation(const BinaryNode& node)
{
    if (!node.left || !node.right)
    {
        throw Error("Unbalanced expression, missing operand", node.token.location, ErrorType::SEMANTIC);
    }

    if (node.token.type == LexerTokenType::DivideToken)
    {
        checkDivisionByZero(*node.right);
    }

    InferredType leftType  = inferType(*node.left);
    InferredType rightType = inferType(*node.right);
    ensureTypeMatch(leftType, rightType, node.token);
    if ((leftType == InferredType::BOOL && rightType == InferredType::BOOL) && isComparisonOp(node))
    {
        throw Error("Invalid operation: cannot compare boolean values using <, >, <=, or >=",
                    node.token.location,
                    ErrorType::SEMANTIC);
    }
    return leftType;
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
    {
        analyzeBlockOperation(*node.elseNode);
    }
}

void Semantic::analyzeBlockOperation(const TreeNode& node)
{
    auto& symbolTable = ScopedSymbolTable::getInstance();
    symbolTable.enterScope();

    if (node.token.type == LexerTokenType::PrintToken)
    {
        analyzePrintOperation(node);
    }
    else
    {

        for (const auto& statement : node.children)
        {
            analyzeTree(*statement);
        }
    }

    symbolTable.exitScope();
}

void Semantic::analyzePrintOperation(const TreeNode& node)
{
    if (node.children.empty())
    {
        throw Error("Print statement requires at least one argument", node.token.location, ErrorType::SEMANTIC);
    }

    // Ensure each child of the print statement is a valid expression.
    for (const auto& child : node.children)
    {
        analyzePrintExpression(*child); // Analyze the child expression.
    }
}

void Semantic::analyzePrintExpression(const ASTNode& node)
{

    if (isSimpleLiteralOrVariable(node))
    {
        return;
    }
    else if (node.getType() == NodeType::BinaryOperation)
    {
        const auto& binaryNode = static_cast<const BinaryNode&>(node);
        if (binaryNode.left && binaryNode.right)
        {
            ensureTypeMatch(inferType(*binaryNode.left), inferType(*binaryNode.right), binaryNode.token);
        }
    }
    else
    {
        throw Error("Invalid expression in print statement", node.token.location, ErrorType::SEMANTIC);
    }
}

bool Semantic::isSimpleLiteralOrVariable(const ASTNode& node) const
{
    return (node.token.type == LexerTokenType::IntToken || node.token.type == LexerTokenType::FloatToken ||
            node.token.type == LexerTokenType::StringToken || node.token.type == LexerTokenType::BoolToken ||
            node.token.type == LexerTokenType::VarToken);
}

void Semantic::checkDivisionByZero(const ASTNode& node)
{
    if (node.token.type == LexerTokenType::IntToken && std::stoi(std::string(node.token.value)) == 0)
    {
        throw Error("Division by zero", node.token.location, ErrorType::SEMANTIC);
    }
    else if (node.token.type == LexerTokenType::FloatToken &&
             std::abs(std::stof(std::string(node.token.value))) < std::numeric_limits<float>::epsilon())
    {
        throw Error("Division by zero", node.token.location, ErrorType::SEMANTIC);
    }
}

bool Semantic::isValidConditionType(const LexerToken& token) const
{
    return (token.type == LexerTokenType::EqualToken || token.type == LexerTokenType::GreaterEqualToken ||
            token.type == LexerTokenType::GreaterToken || token.type == LexerTokenType::LessEqualToken ||
            token.type == LexerTokenType::LessToken || token.type == LexerTokenType::NotEqualToken);
}

bool Semantic::isValidBinaryType(const LexerToken& token) const
{
    return (token.type == LexerTokenType::PlusToken || token.type == LexerTokenType::MinusToken ||
            token.type == LexerTokenType::MultiplyToken || token.type == LexerTokenType::DivideToken);
}

std::string Semantic::getVariableName(const ASTNode& node) const
{
    return std::string(node.token.value);
}

nlohmann::json Semantic::tableToJson(const symbolTable& table)
{
    nlohmann::json jArray = nlohmann::json::array();
    for (const auto& scope : table)
    {
        nlohmann::json scopeJson = nlohmann::json::object();
        for (const auto& [name, info] : scope)
        {
            scopeJson[name] = {
                {"type", getInferredTypeDescription(info.type)},
                {"value", name},
            };
        }
        jArray.push_back(scopeJson);
    }
    return jArray;
}

constexpr std::string_view Semantic::getInferredTypeDescription(const InferredType& t)
{
    switch (t)
    {
    case InferredType::BOOL:
        return "Boolean";
    case InferredType::FLOAT:
        return "Float";
    case InferredType::INTEGER:
        return "Integer";
    case InferredType::STRING:
        return "String";
    default:
        return "An unknown error occurred";
    }
}

void Semantic::addSymbolTableToOutput()
{
    m_output.getJson()["SymbolTable"] = tableToJson(ScopedSymbolTable::getInstance().getSymbolTable());
}