#pragma once

#include <cstddef>
#include <span>
#include "helpers/type.hpp"
#include "lexer/lexer.hpp"

namespace cx
{

    enum class NodeKind
    {
        Program,

        BinaryExpr,
        UnaryExpr,

        IfStmt,
        WhileStmt,
        ReturnStmt,

        Identifier,
        String,
        Number,
        Bool,

        FuncDecl,
        FuncCall,

        Print,

        Error,
    };

    struct Node
    {
        cx::Token token;
        NodeKind  kind;
        ValueType valuetype = ValueType::Unknown;

        //! Identifiers only
        std::size_t slot = static_cast<std::size_t>(-1);

        Node(Token token, NodeKind kind, ValueType valuetype) : token(token), kind(kind), valuetype(valuetype) {}
    };

    struct BinaryNode : Node
    {
        Node* left;
        Node* right;

        BinaryNode(Token token, NodeKind kind,  ValueType valuetype,  Node* left, Node* right) : Node(token, kind, valuetype), left(left), right(right) {}
    };

    struct UnaryNode : Node
    {
        Node* operand;

        UnaryNode(Token token, NodeKind kind, ValueType valuetype, Node* operand) : Node(token, kind, valuetype), operand(operand) {}
    };

    struct IfNode : Node
    {
        Node*            condition;
        std::span<Node*> then;
        std::span<Node*> nelse;

        IfNode(Token token, NodeKind kind, ValueType valuetype, Node* cond, std::span<Node*> then, std::span<Node*> nelse)
            : Node(token, kind, valuetype), condition(cond), then(then), nelse(nelse)
        {
        }
    };

    struct WhileNode : Node
    {
        Node*            condition;
        std::span<Node*> loop;

        WhileNode(Token token, NodeKind kind, ValueType valuetype, Node* cond, std::span<Node*> loop) : Node(token, kind, valuetype), condition(cond), loop(loop) {}
    };

    // function definition
    struct FuncNode : Node
    {
        Node*            name;
        std::span<Node*> parameters;
        std::span<Node*> body;

        FuncNode(Token token, NodeKind kind, ValueType valuetype, Node* name, std::span<Node*> parameters, std::span<Node*> body)
            : Node(token, kind, valuetype), name(name), parameters(parameters), body(body)
        {
        }
    };

    // functioncall
    struct CallNode : Node
    {
        Node*            callee;
        std::span<Node*> arguments;
        CallNode(Token token, NodeKind kind, ValueType valuetype, Node* callee, std::span<Node*> arguments) : Node(token, kind, valuetype), callee(callee), arguments(arguments) {}
    };

    struct ProgramNode : Node
    {
        std::span<Node*> statements;
        ProgramNode(Token token, NodeKind kind, ValueType valuetype, std::span<Node*> statements) : Node(token, kind, valuetype), statements(statements) {}
    };

    struct PrintNode : Node
    {
        Node* expression;

        PrintNode(Token token, NodeKind kind, ValueType valuetype, Node* expression) : Node(token, kind, valuetype), expression(expression) {}
    };

    struct ReturnNode : Node
    {
        Node* expression;

        ReturnNode(Token token, NodeKind kind, ValueType valuetype, Node* expression) : Node(token, kind, valuetype), expression(expression) {}
    };
} // namespace cx

// chane name to elseblock