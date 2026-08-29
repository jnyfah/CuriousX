#pragma once

#include "lexer/lexer.hpp"

namespace cx
{

    enum class NodeType
    {
        Program,

        BinaryExpr,
        UnaryExpr,
        CondExpr,
        CallExpr,

        IfStmt,
        WhileStmt,
        ReturnStmt,
        ForStmt,

        Identifier,
        String,
        Number,
        Bool,

        FuncDecl,
        Comment,
    };

    struct Node
    {
        cx::Token token;
        NodeType  type;
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
        std::vector<Node*> thenBody;
        std::vector<Node*> elseBody;

        IfNode(Token token, NodeType type, Node* cond, std::vector<Node*> then, std::vector<Node*> els)
            : Node(token, type), condition(cond), thenBody(then), elseBody(els)
        {
        }
    };

    struct CallNode : Node
    {
        Node*              callee;
        std::vector<Node*> arguments;
        size_t             argumentCount;
    };

    struct ProgramNode : Node
    {
        std::vector<Node*> statements;
        size_t             statementCount;
    };

    // how do we represent the root node??
    // what is the entry point of this parser ??
    // does number literal include float and ints both of them
} // namespace cx


// chane name to elseblock