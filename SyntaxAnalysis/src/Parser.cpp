#include "SyntaxAnalysis/include/Parser.hpp"
#include <iostream>


bool Parser::Parse()
{
  if (current >= token.size()) { return false; }

  // parse multiple lines
  while (current < token.size()) {
    root = Expression();
    compound.push_back(std::move(root));
  }

  // if nothing is parsed
  if (compound.empty()) { return false; }
  return true;
}




/**
 * @ Expression is made of sum of Terms
 *
 * E -> T+E || T-E || T
 */

std::unique_ptr<Node> Parser::Expression()
{
  std::unique_ptr<Node> left = Term();

  while (true) {
    if (current >= token.size()) { return left; }

    if ((token[current].type == LexerTokenType::PlusToken) || (token[current].type == LexerTokenType::MinusToken)) {
      LexerToken type = token[current];
      current++;

      if (current >= token.size()) { return left; }

      std::unique_ptr<Node> right = Term();
      left = makeNode(std::move(left), std::move(right), type);

    } else {
      return left;
    }
  }
}




/**
 * @ Term is a product of factors
 *
 * T -> F*T || F/T || F
 */

std::unique_ptr<Node> Parser::Term()
{
  std::unique_ptr<Node> left = Factor();

  // Parse assign token
  if (token[current].type == LexerTokenType::AssignToken) {
    LexerToken type = token[current];
    current++;

    std::unique_ptr<Node> right = Expression();
    left = makeNode(std::move(left), std::move(right), type);

    return left;
  }

  while (true) {
    if (current >= token.size()) { return left; }

    if ((token[current].type == LexerTokenType::MultiplyToken)
        || (token[current].type == LexerTokenType::DivideToken)) {
      LexerToken type = token[current];
      current++;

      if (current >= token.size()) { return left; }

      std::unique_ptr<Node> right = Factor();
      left = makeNode(std::move(left), std::move(right), type);

    } else {
      return left;
    }
  }
}




/**
 * @ Factor is a number, string or parenthesized sub expression
 *
 * F -> ID || Integer || E
 */
std::unique_ptr<Node> Parser::Factor()
{
  std::unique_ptr<Node> node = std::make_unique<Node>();

  if (current >= token.size()) { return node; }

  // To do variable name, if,
  if ((token[current].type == LexerTokenType::IntToken) || (token[current].type == LexerTokenType::FloatToken)
      || (token[current].type == LexerTokenType::VarToken)) {
    node = makeLeaf(token[current]);
    current++;
    return node;
  }
  // if parenthesized sub expression
  else if (token[current].type == LexerTokenType::ParenOpen) {
    current++;
    std::unique_ptr<Node> left = Expression();

    if (token[current].type == LexerTokenType::ParenClose) {
      current++;
      return left;

    } else {
      throw Error("no closing braces", left->type.location);
    }
  } else if (token[current].type == LexerTokenType::PrintToken) {
    return Print();
  }

  return node;
}




/**
 * @ Print out a number, string, variable or even expression
 *
 * Print -> ID || Integer || E || String
 */

std::unique_ptr<Node> Parser::Print()
{
  std::unique_ptr<Node> left = std::make_unique<Node>();

  LexerToken type = token[current];
  current++;

  // check for parenthesis
  if (token[current].type != LexerTokenType::ParenOpen) {
    throw Error("no opening braces after print", token[current].location);
  }
  current++;
  left = Expression();

  // ensure parenthesis is closed
  if (token[current].type != LexerTokenType::ParenClose) {
    throw Error("no closing braces after print", token[current].location);
  }
  current++;
  return makeUnary(std::move(left), type);
}


/*
 check for if
 check for paren
*/