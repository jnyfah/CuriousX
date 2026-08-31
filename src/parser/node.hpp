#pragma once

#include "lexer/lexer.hpp"

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
        virtual ~Node() = default;
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
        std::vector<Node*> then;
        std::vector<Node*> nelse;

        IfNode(Token token, NodeType type, Node* cond, std::vector<Node*> then, std::vector<Node*> nelse)
            : Node(token, type), condition(cond), then(then), nelse(nelse)
        {
        }
    };

    struct WhileNode : Node
    {
        Node*              condition;
        std::vector<Node*> loop;
        WhileNode(Token token, NodeType type, Node* cond, std::vector<Node*> loop) : Node(token, type), condition(cond), loop(loop) {}
    };

    // function definition
    struct FuncNode : Node
    {
        Node*              name;
        std::vector<Node*> parameters;
        std::vector<Node*> body;

        FuncNode(Token token, NodeType type, Node* name, std::vector<Node*> parameters, std::vector<Node*> body)
            : Node(token, type), name(name), parameters(parameters), body(body)
        {
        }
    };

    // functioncall
    struct CallNode : Node
    {
        Node*              callee;
        std::vector<Node*> arguments;
        CallNode(Token token, NodeType type, Node* callee, std::vector<Node*> arguments) : Node(token, type), callee(callee), arguments(arguments) {}
    };

    struct ProgramNode : Node
    {
        std::vector<Node*> statements;
        ProgramNode(Token token, NodeType type, std::vector<Node*> statements) : Node(token, type), statements(statements) {}
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