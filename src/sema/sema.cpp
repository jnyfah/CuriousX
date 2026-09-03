#include "sema.hpp"
#include "helpers/diagnostics.hpp"
#include "helpers/type.hpp"
#include "lexer/token.hpp"
#include "parser/node.hpp"
#include "sema/symboltable.hpp"

namespace cx
{

    namespace
    {
        bool isArithmetic(TokenType t)
        {
            return t == TokenType::Plus || t == TokenType::Minus || t == TokenType::Multiply || t == TokenType::Divide;
        }

        bool isComparison(TokenType t)
        {
            return t == TokenType::Less || t == TokenType::LessEqual || t == TokenType::Greater || t == TokenType::GreaterEqual;
        }

        bool isEquality(TokenType t)
        {
            return t == TokenType::Equal || t == TokenType::NotEqual;
        }

        bool isLogical(TokenType t)
        {
            return t == TokenType::And || t == TokenType::Or;
        }

        bool isNumeric(ValueType t)
        {
            return t == ValueType::Int || t == ValueType::Float;
        }

        bool isBoolean(ValueType t)
        {
            return t == ValueType::Bool;
        }
    } // namespace

    Sema::Sema(Diagnostics& d) : m_diag(d), m_table(m_diag) {}
    void Sema::analyze(ProgramNode* root)
    {

        for (auto& statement : root->statements)
        {
            analyzeStmt(statement);
        }
    }

    void Sema::analyzeStmt(Node* node)
    {
        switch (node->kind)
        {
            case NodeKind::IfStmt:
            case NodeKind::ReturnStmt:
            case NodeKind::WhileStmt:
            case NodeKind::Print:
            case NodeKind::FuncDecl:
            default:
                analyzeExpr(node);
        }
    }

    ValueType Sema::analyzeExpr(Node* node)
    {

        switch (node->kind)
        {
            case NodeKind::Number:
                (node->token.type == TokenType::Int) ? node->valuetype = ValueType::Int : node->valuetype = ValueType::Float;
                return node->valuetype;
            case NodeKind::String:
                node->valuetype = ValueType::String;
                return ValueType::String;
            case NodeKind::Bool:
                node->valuetype = ValueType::Bool;
                return ValueType::Bool;
            case NodeKind::Identifier:
                inferIdentifier(node);
            case NodeKind::BinaryExpr:
                return analyzeBinary(static_cast<BinaryNode*>(node));
            case NodeKind::UnaryExpr:
                return analyzeUnary(static_cast<UnaryNode*>(node));
            case NodeKind::FuncCall:
            case NodeKind::Error:
            default:
                return ValueType::Error;
        }
    }

    ValueType Sema::inferIdentifier(Node* node)
    {
        if (const auto info = m_table.lookup(node->token.value))
        {
            node->slot      = info->slot;
            node->valuetype = info->valuetype;
            return node->valuetype;
        }

        m_diag.error(node->token.location, "undefined variable '{}'", node->token.value);

        node->valuetype = ValueType::Error;
        return ValueType::Error;
    }

    ValueType Sema::analyzeBinary(BinaryNode* node)
    {
        if (node->token.type == TokenType::Assign)
        {
            // right first: its type is what the variable is declared as
            const ValueType right = analyzeExpr(node->right);

            if (node->left->kind != NodeKind::Identifier)
            {
                m_diag.error(node->left->token.location, "left side of an assignment must be a variable");
                node->valuetype = ValueType::Error;
                return ValueType::Error;
            }

            // insert() reports the mismatch when the variable already exists with another type.
            m_table.insert(node->left->token.value, right, node->left->token);

            node->left->valuetype = right;
            node->valuetype       = right;
            return right;
        }

        const ValueType left  = analyzeExpr(node->left);
        const ValueType right = analyzeExpr(node->right);
        const TokenType op    = node->token.type;

        // already reported further down the tree
        if (left == ValueType::Error || right == ValueType::Error)
        {
            node->valuetype = ValueType::Error;
            return ValueType::Error;
        }

        if (left != right)
        {
            // TODO: Int/Float stops being a mismatch soon!!
            m_diag.error(node->token.location, "cannot apply '{}' to {} and {}", describe(op), describe(left), describe(right));
            node->valuetype = ValueType::Error;
            return ValueType::Error;
        }

        // does this operator accept this type?
        const ValueType operand = left;
        ValueType       result  = ValueType::Error;

        if (isArithmetic(op))
        {
            result = isNumeric(operand) ? operand : ValueType::Error;
        }
        else if (op == TokenType::Percent)
        {
            // no float remainder
            result = (operand == ValueType::Int) ? ValueType::Int : ValueType::Error;
        }
        else if (isComparison(op))
        {
            result = isNumeric(operand) ? ValueType::Bool : ValueType::Error;
        }
        else if (isEquality(op))
        {
            result = ValueType::Bool;
        }
        else if (isLogical(op))
        {
            result = (operand == ValueType::Bool) ? ValueType::Bool : ValueType::Error;
        }
        else
        {
            m_diag.error(node->token.location, "'{}' is not a binary operator", describe(op));
            node->valuetype = ValueType::Error;
            return ValueType::Error;
        }

        if (result == ValueType::Error)
        {
            m_diag.error(node->token.location, "operator '{}' cannot be applied to {}", describe(op), describe(operand));
        }

        node->valuetype = result;
        return result;
    }

    ValueType Sema::analyzeUnary(UnaryNode* node)
    {

        ValueType result  = ValueType::Error;
        auto      operand = analyzeExpr(node->operand);
        if (node->token.type == TokenType::Minus)
        {
            result = isNumeric(operand) ? operand : ValueType::Error;
        }
        else if (node->token.type == TokenType::Not)
        {
            result = isBoolean(operand) ? operand : ValueType::Error;
        }

        node->valuetype = result;
        return result;
    }

} // namespace cx

// analyze(ProgramNode*)
//     collectFunctions(...)          // pass 1: declareFunction over top-level FuncDecls
//     for each statement: analyzeStmt

// analyzeStmt(Node*)   switch (n->type)
//     IfStmt      → condition must be Bool; analyze both bodies in new scopes
//     WhileStmt   → same
//     Print       → analyzeExpr, reject Void
//     ReturnStmt  → analyzeExpr, compare against current function's returnType
//     FuncDecl    → beginFunction / analyze body / endFunction
//     default     → analyzeExpr (expression statement)

// analyzeExpr(Node*) → Type   switch (n->type)
//     Number      → Int or Float, from the token type
//     String/Bool → the obvious
//     Identifier  → lookup; stamp the slot on the node; unknown name is an error
//     BinaryExpr  → assignment vs operator; operand types must agree
//     UnaryExpr   → '-' needs numeric, '!' needs Bool
//     FuncCall    → findFunction, check arity, check each argument, return returnType
//     Error       → Type::Error, silently
