#include "sema.hpp"
#include <cstddef>
#include <vector>
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

        bool isExpression(ValueType t)
        {
            return t != ValueType::Void;
        }
    } // namespace


    Sema::Sema(Diagnostics& d) : m_diag(d), m_table(m_diag) {}


    void Sema::analyze(ProgramNode* root)
    {
        // pass 1: every top-level function is declared before any body is walked,
        // so a call may name a function declared further down the file
        for (Node* statement : root->statements)
        {
            if (statement->kind == NodeKind::FuncDecl)
            {
                analyzeFunDclr(static_cast<FuncNode*>(statement));
            }
        }

        // top-level statements are the body of an implicit function, so that every
        // variable in the program belongs to a frame and gets a real slot
        const Token mainToken{"@main", Location{0, 0}, TokenType::Var};
        m_table.declareFunction("@main", mainToken, {}, ValueType::Void, nullptr);

        const std::size_t mainIndex = m_table.findFunction("@main");
        m_table.beginFunction(mainIndex);

        // pass 2: function bodies are walked here, once the argument types have bound the parameters
        for (Node* statement : root->statements)
        {
            if (statement->kind != NodeKind::FuncDecl)
            {
                analyzeStmt(statement);
            }
        }

        m_table.endFunction();

        for (const FunctionInfo& f : m_table.functions())
        {
            if (f.state == Analysis::NotStarted && f.name != "@main")
            {
                m_diag.warning(f.token.location, "function '{}' is never called", f.name);
            }
        }
    }

    void Sema::analyzeStmt(Node* node)
    {
        switch (node->kind)
        {
            case NodeKind::IfStmt:
                analyzeIf(static_cast<IfNode*>(node));
                break;
            case NodeKind::ReturnStmt:
                analyzeReturn(static_cast<ReturnNode*>(node));
                break;
            case NodeKind::WhileStmt:
                analyzeWhile(static_cast<WhileNode*>(node));
                break;
            case NodeKind::Print:
                analyzePrint(static_cast<PrintNode*>(node));
                break;
            case NodeKind::FuncDecl:
                // declared in pass 1; the body waits for the first call
                break;
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
                return inferIdentifier(node);
            case NodeKind::BinaryExpr:
                return analyzeBinary(static_cast<BinaryNode*>(node));
            case NodeKind::UnaryExpr:
                return analyzeUnary(static_cast<UnaryNode*>(node));
            case NodeKind::FuncCall:
                return analyzeFunCall(static_cast<CallNode*>(node));
            case NodeKind::Error:
                return ValueType::Error;
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
            node->left->slot      = m_table.insert(node->left->token.value, right, node->left->token);
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
        const ValueType operand = analyzeExpr(node->operand);

        if (operand == ValueType::Error)
        {
            node->valuetype = ValueType::Error;
            return ValueType::Error;
        }

        ValueType result = ValueType::Error;

        if (node->token.type == TokenType::Minus)
        {
            result = isNumeric(operand) ? operand : ValueType::Error;
        }
        else if (node->token.type == TokenType::Not)
        {
            result = isBoolean(operand) ? ValueType::Bool : ValueType::Error;
        }
        else
        {
            m_diag.error(node->token.location, "'{}' is not a unary operator", describe(node->token.type));
            node->valuetype = ValueType::Error;
            return ValueType::Error;
        }

        if (result == ValueType::Error)
        {
            m_diag.error(node->token.location, "operator '{}' cannot be applied to {}", describe(node->token.type), describe(operand));
        }

        node->valuetype = result;
        return result;
    }

    void Sema::analyzeIf(IfNode* node)
    {
        const auto cond = analyzeExpr(node->condition);

        if (cond != ValueType::Bool && cond != ValueType::Error)
        {
            m_diag.error(node->condition->token.location, "if condition must be a boolean, found {}", describe(cond));
        }

        m_table.enterScope();
        for (auto n : node->then)
        {
            analyzeStmt(n);
        }
        m_table.exitScope();

        if (!node->nelse.empty())
        {
            m_table.enterScope();
            for (auto n : node->nelse)
            {
                analyzeStmt(n);
            }
            m_table.exitScope();
        }
    }

    void Sema::analyzeWhile(WhileNode* node)
    {
        const auto cond = analyzeExpr(node->condition);

        if (cond != ValueType::Bool && cond != ValueType::Error)
        {
            m_diag.error(node->condition->token.location, "while condition must be a boolean, found {}", describe(cond));
        }

        m_table.enterScope();
        for (auto n : node->loop)
        {
            analyzeStmt(n);
        }
        m_table.exitScope();
    }

    // todo fix
    void Sema::analyzeReturn(ReturnNode* node)
    {
        // a bare `return;` yields nothing
        const ValueType returned = node->expression ? analyzeExpr(node->expression) : ValueType::Void;
        node->valuetype          = returned;

        const std::size_t index  = m_table.currentFunction();
        if (index == SymbolTable::kNoFunction)
        {
            m_diag.error(node->token.location, "'return' outside of a function");
            return;
        }

        // already reported inside the expression
        if (returned == ValueType::Error)
        {
            return;
        }

        FunctionInfo& funcInfo = m_table.function(index);

        // has a return function been set already, do they match ?
        if (funcInfo.returnType == ValueType::Unknown)
        {
            funcInfo.returnType = returned;
            return;
        }

        if (returned != funcInfo.returnType)
        {
            m_diag.error(
                node->token.location,
                "cannot return {} from '{}', which returns {}",
                describe(returned),
                funcInfo.name,
                describe(funcInfo.returnType));
        }
    }

    void Sema::analyzePrint(PrintNode* node)
    {
        const auto expr = analyzeExpr(node->expression);

        // print is always an expression,
        if (expr == ValueType::Error)
        {
            // no need to report this
            return;
        } // cant print a void
        else if (expr == ValueType::Void)
        {
            m_diag.error(node->expression->token.location, "cannot print a value of type {}", describe(expr));
        }
    }

    ValueType Sema::analyzeFunCall(CallNode* node)
    {
        const std::size_t index = m_table.findFunction(node->callee->token.value);
        if (index == SymbolTable::kNoFunction)
        {
            m_diag.error(node->callee->token.location, "undefined function '{}'", node->callee->token.value);

            // still analyze the arguments so their subexpressions get types and slots
            for (Node* arg : node->arguments)
            {
                analyzeExpr(arg);
            }

            node->valuetype = ValueType::Error;
            return ValueType::Error;
        }

        auto& funcInfo = m_table.function(index);

        if (funcInfo.paramTypes.size() != node->arguments.size())
        {
            m_diag.error(
                node->token.location,
                "'{}' expects {} argument(s) but {} were given; declared on line {}",
                funcInfo.name,
                funcInfo.paramTypes.size(),
                node->arguments.size(),
                funcInfo.token.location.getLine());

            for (Node* arg : node->arguments)
            {
                analyzeExpr(arg);
            }

            node->valuetype = ValueType::Error;
            return ValueType::Error;
        }

        bool ok = true;

        for (std::size_t i = 0; i < node->arguments.size(); ++i)
        {
            const auto arg = analyzeExpr(node->arguments[i]);

            if (arg == ValueType::Error)
            {
                ok = false;
                continue;
            }

            if (funcInfo.paramTypes[i] == ValueType::Unknown)
            {
                funcInfo.paramTypes[i] = arg;
            }
            else if (arg != funcInfo.paramTypes[i])
            {
                m_diag.error(
                    node->arguments[i]->token.location,
                    "argument {} of '{}': expected {}, found {}",
                    i + 1,
                    funcInfo.name,
                    describe(funcInfo.paramTypes[i]),
                    describe(arg));
                ok = false;
            }
        }

        analyzeFunctionBody(index);

        node->valuetype = ok ? funcInfo.returnType : ValueType::Error;
        return node->valuetype;
    }

    void Sema::analyzeFunDclr(FuncNode* node)
    {
        std::vector<ValueType> params(node->parameters.size(), ValueType::Unknown);
        m_table.declareFunction(node->name->token.value, node->token, params, ValueType::Unknown, node);
    }

    void Sema::analyzeFunctionBody(size_t index)
    {
        FunctionInfo& funcInfo = m_table.function(index);

        // InProgress: a recursive call reached us while the body is still being walked.
        // Done: a later call site; the arguments were checked, the body needs no rewalk.
        if (funcInfo.state != Analysis::NotStarted)
        {
            return;
        }

        if (funcInfo.decl == nullptr)
        {
            return;
        }

        funcInfo.state = Analysis::InProgress;

        m_table.beginFunction(index);
        for (Node* statement : funcInfo.decl->body)
        {
            analyzeStmt(statement);
        }
        m_table.endFunction();

        // no return statement anywhere in the body
        if (funcInfo.returnType == ValueType::Unknown)
        {
            funcInfo.returnType = ValueType::Void;
        }

        funcInfo.state = Analysis::Done;
    }

} // namespace cx
