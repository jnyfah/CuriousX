#include "Parser.hpp"


namespace Parser {

bool Parser::Parser::Parse()
{
  if (token[current].type == LexerTokenType::Eof) { return false; }

  root = Expression();
  if (root == nullptr) { return false; }

  return true;
}


std::unique_ptr<Node> Parser::Expression()
{
  std::unique_ptr<Node> node;

  node->left = Term();

  if (token[current].type == LexerTokenType::Eof) { return nullptr; }

  if ((token[current].type == LexerTokenType::PlusToken) || (token[current].type == LexerTokenType::MinusToken)) {
    node->type = token[current];
    current++;
    node->right = Term();
  }
  return node;
}


std::unique_ptr<Node> Parser::Term()
{
  std::unique_ptr<Node> node;

  node->left = Factor();

  if (token[current].type == LexerTokenType::Eof) { return nullptr; }

  if ((token[current].type == LexerTokenType::MultiplyToken) || (token[current].type == LexerTokenType::DivideToken)) {
    node->type = token[current];
    current++;
    node->right = Factor();
  }
  return node;
}


std::unique_ptr<Node> Parser::Factor()
{
  std::unique_ptr<Node> node;

  // To do variable name, if,
  if ((token[current].type == LexerTokenType::IntToken) || (token[current].type == LexerTokenType::FloatToken)) {
    node = makeLeaf(token[current]);
    current++;
    return node;
  }
  return node;
}


void Parser::printAst() { inOrder(std::move(root)); }

void Parser::inOrder(std::unique_ptr<Node> node)
{
  if (node) {
    std::cout << node->type.value << std::endl;

    inOrder(std::move(root->left));

    inOrder(std::move(root->right));
  }
}


};// namespace Parser


// 7 + 8

// 2 * 3 + 4 * 5


// let

// + 8

// print

// string

// unknown

// if