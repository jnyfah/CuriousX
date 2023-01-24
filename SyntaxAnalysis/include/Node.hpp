#ifndef NODE_HPP
#define NODE_HPP

#include <iomanip>
#include <memory>
#include <queue>
#include <variant>

#include "Error.hpp"
#include "LexicalAnalysis/include/LexerToken.hpp"

enum class InferredType { INTEGER, FLOAT };

struct Node
{
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
    LexerToken type;
    std::variant<InferredType, std::monostate> inferredType;
};

// Build and return a generic AST node
inline std::shared_ptr<Node> makeNode(std::shared_ptr<Node> left, std::shared_ptr<Node> right, const LexerToken &type)
{
    std::shared_ptr<Node> node = std::make_shared<Node>();

    node->left = left;
    node->right = right;
    node->type = type;
    node->inferredType = std::monostate{};

    return node;
}

// Make an AST leaf node
inline std::shared_ptr<Node> makeLeaf(const LexerToken &type) { return (makeNode(nullptr, nullptr, type)); }

// Make a unary AST node: only one child
inline std::shared_ptr<Node> makeUnary(std::shared_ptr<Node> left, const LexerToken &type)
{
    return (makeNode(left, nullptr, type));
}

// Get height of tree
inline int TreeHeight(std::shared_ptr<Node> node)
{
    if (!node) return 0;
    return 1 + std::max(TreeHeight(node->left), TreeHeight(node->right));
}

#endif