#pragma once

#include "lexer/lexer.hpp"
#include <span>

namespace cx
{

    enum class NodeType
    {
        Program,

        BinaryExpr,
        UnaryExpr,

        IfStmt,
        WhileStmt,
        ReturnStmt,
        ForStmt,

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
        NodeType  type;

        Node(Token token, NodeType type) : token(token), type(type) {}
    };

    struct BinaryNode : Node
    {
        Node* left;
        Node* right;

        BinaryNode(Token token, NodeType type, Node* left, Node* right) : Node(token, type), left(left), right(right) {}
    };

    struct UnaryNode : Node
    {
        Node* operand;

        UnaryNode(Token token, NodeType type, Node* operand) : Node(token, type), operand(operand) {}
    };

    struct IfNode : Node
    {
        Node*              condition;
        std::span<Node*> then;
        std::span<Node*> nelse;

        IfNode(Token token, NodeType type, Node* cond, std::span<Node*> then, std::span<Node*> nelse)
            : Node(token, type), condition(cond), then(then), nelse(nelse)
        {
        }
    };

    struct WhileNode : Node
    {
        Node*              condition;
        std::span<Node*> loop;
        WhileNode(Token token, NodeType type, Node* cond, std::span<Node*> loop) : Node(token, type), condition(cond), loop(loop) {}
    };

    // function definition
    struct FuncNode : Node
    {
        Node*              name;
        std::span<Node*> parameters;
        std::span<Node*> body;

        FuncNode(Token token, NodeType type, Node* name, std::span<Node*> parameters, std::span<Node*> body)
            : Node(token, type), name(name), parameters(parameters), body(body)
        {
        }
    };

    // functioncall
    struct CallNode : Node
    {
        Node*              callee;
        std::span<Node*> arguments;
        CallNode(Token token, NodeType type, Node* callee, std::span<Node*> arguments) : Node(token, type), callee(callee), arguments(arguments) {}
    };

    struct ProgramNode : Node
    {
        std::span<Node*> statements;
        ProgramNode(Token token, NodeType type, std::span<Node*> statements) : Node(token, type), statements(statements) {}
    };

    struct PrintNode : Node
    {
        Node* expression;

        PrintNode(Token token, NodeType type, Node* expression) : Node(token, type), expression(expression) {}
    };

    struct ReturnNode : Node
    {
        Node* expression;

        ReturnNode(Token token, NodeType type, Node* expression) : Node(token, type), expression(expression) {}
    };
} // namespace cx

// chane name to elseblock