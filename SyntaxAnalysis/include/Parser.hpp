#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>

#include "Error.hpp"
#include "Node.hpp"


namespace Parser {

class Parser
{


public:
  explicit Parser(const std::vector<LexerToken> &token) : token(token)
  {
    root = nullptr;
    current = 0;
  }

  std::unique_ptr<Node> Expression();

  std::unique_ptr<Node> Term();

  std::unique_ptr<Node> Factor();

  std::unique_ptr<Node> Assign();

  bool Parse();

  void printAst();

  void inOrder(std::unique_ptr<Node> node);

private:
  std::unique_ptr<Node> root;

  std::vector<LexerToken> token;
  size_t current;


  int level;
};


};// namespace Parser

#endif