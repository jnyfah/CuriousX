#ifndef NODE_HPP
#define NODE_HPP

#include <iomanip>
#include <memory>
#include <queue>


#include "Error.hpp"
#include "LexicalAnalysis/include/LexerToken.hpp"

struct Node
{
  std::unique_ptr<Node> left;
  std::unique_ptr<Node> right;
  LexerToken type;
};


// Build and return a generic AST node
inline std::unique_ptr<Node> makeNode(std::unique_ptr<Node> left, std::unique_ptr<Node> right, const LexerToken &type)
{
  std::unique_ptr<Node> node = std::make_unique<Node>();

  node->left = std::move(left);
  node->right = std::move(right);
  node->type = type;

  return node;
}

// Make an AST leaf node
inline std::unique_ptr<Node> makeLeaf(const LexerToken &type) { return (makeNode(nullptr, nullptr, type)); }

// Make a unary AST node: only one child
inline std::unique_ptr<Node> makeUnary(std::unique_ptr<Node> left, const LexerToken &type)
{
  return (makeNode(std::move(left), nullptr, type));
}

// Get height of tree
inline int TreeHeight(std::unique_ptr<Node> &node)
{
  if (!node) return 0;
  return 1 + std::max(TreeHeight(node->left), TreeHeight(node->right));
}


#endif