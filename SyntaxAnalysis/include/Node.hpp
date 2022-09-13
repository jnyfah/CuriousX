#ifndef NODE_HPP
#define NODE_HPP

#include <memory>


#include "LexicalAnalysis/include/Lexer.hpp"

struct Node
{
  std::unique_ptr<Node> left;
  std::unique_ptr<Node> right;
  LexerToken type;
};


// Build and return a generic AST node
std::unique_ptr<Node> makeNode(std::unique_ptr<Node> left, std::unique_ptr<Node> right, LexerToken type)
{
  std::unique_ptr<Node> node;

  node->left = std::move(left);
  node->right = std::move(right);
  node->type = type;

  return node;
}

// Make an AST leaf node
std::unique_ptr<Node> makeLeaf(LexerToken type) { return (makeNode(nullptr, nullptr, type)); }

// Make a unary AST node: only one child
std::unique_ptr<Node> makeUnary(std::unique_ptr<Node> left, LexerToken type)
{
  return (makeNode(std::move(left), nullptr, type));
}


#endif