#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>

#include "Error.hpp"
#include "Node.hpp"


class Parser
{

public:
  explicit Parser(const std::vector<LexerToken> &token) : token(token)
  {
    root = std::make_unique<Node>();
    current = 0;
  }

  std::unique_ptr<Node> root;

  std::unique_ptr<Node> Expression();

  std::unique_ptr<Node> Term();

  std::unique_ptr<Node> Factor();

  std::unique_ptr<Node> Assign();

  bool Parse();

  void printAst();

  void inOrder(std::unique_ptr<Node> node);

private:
  // std::unique_ptr<Node> root;

  std::vector<LexerToken> token;
  size_t current;


  int level;
};

void displayInOrder(std::unique_ptr<Node> nodePtr)
{
  if (nodePtr) {
    displayInOrder(std::move(nodePtr->left));
    std::cout << nodePtr->type.value << std::endl;
    displayInOrder(std::move(nodePtr->right));
  }
}

void printTree(std::unique_ptr<Node> root, int space)
{
  if (root == NULL) return;
  space += 1;
  printTree(std::move(root->right), space);
  for (int i = 1; i < space; i++) std::cout << "\t";
  std::cout << root->type.value << "\n";
  printTree(std::move(root->left), space);
}


#endif