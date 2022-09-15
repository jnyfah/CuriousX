#include "SyntaxAnalysis/include/Parser.hpp"
#include <iostream>

bool Parser::Parse()
{
  if (current >= token.size()) { return false; }

  root = Expression();
  if (root == nullptr) { return false; }

  return true;
}


std::unique_ptr<Node> Parser::Expression()
{

  std::unique_ptr<Node> node = std::make_unique<Node>();

  std::unique_ptr<Node> a = Term();

  while (true) {
    if (current >= token.size()) { return a; }
    if ((token[current].type == LexerTokenType::PlusToken) || (token[current].type == LexerTokenType::MinusToken)) {
      node->type = token[current];
      current++;
      if (current >= token.size()) { return a; }
      std::unique_ptr<Node> b = Term();
      a = makeNode(std::move(a), std::move(b), node->type);
    } else {
      return a;
    }
  }
}

std::unique_ptr<Node> Parser::Term()
{

  std::unique_ptr<Node> node = std::make_unique<Node>();

  std::unique_ptr<Node> a = Factor();

  while (true) {
    if (current >= token.size()) { return a; }
    if ((token[current].type == LexerTokenType::MultiplyToken)
        || (token[current].type == LexerTokenType::DivideToken)) {
      node->type = token[current];
      current++;
      if (current >= token.size()) { return a; }
      std::unique_ptr<Node> b = Factor();
      a = makeNode(std::move(a), std::move(b), node->type);
    } else {
      return a;
    }
  }
}


std::unique_ptr<Node> Parser::Factor()
{
  std::unique_ptr<Node> node = std::make_unique<Node>();

  if (current >= token.size()) { return node; }

  // To do variable name, if,
  if ((token[current].type == LexerTokenType::IntToken) || (token[current].type == LexerTokenType::FloatToken)) {
    node = makeLeaf(token[current]);
    current++;
    return node;
  }
  return node;
}

void Parser::displayInOrder(std::unique_ptr<Node> nodePtr)
{
  if (nodePtr) {
    displayInOrder(std::move(nodePtr->left));
    std::cout << nodePtr->type.value << std::endl;
    displayInOrder(std::move(nodePtr->right));
  }
}

void Parser::printTree(std::unique_ptr<Node> root, int space)
{
  if (root == NULL) return;
  space += 1;
  printTree(std::move(root->right), space);
  for (int i = 1; i < space; i++) std::cout << "\t";
  std::cout << root->type.value << "\n";
  printTree(std::move(root->left), space);
}
// 7 + 8

// 2 * 3 + 4 * 5


// let

// + 8

// print

// string

// unknown

// if

// Todo
// add parenopen
// add asignment