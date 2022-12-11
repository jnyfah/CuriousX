#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>

#include "Error.hpp"
#include "Node.hpp"


class Parser
{
  public:
    explicit Parser(const std::vector<LexerToken>& token)
      : token(token)
    {
        root = std::make_unique<Node>();
        current = 0;
    }

    std::unique_ptr<Node> root;

    std::unique_ptr<Node> Expression();

    std::unique_ptr<Node> Term();

    std::unique_ptr<Node> Factor();

    std::unique_ptr<Node> Print();

    std::unique_ptr<Node> Assign(std::unique_ptr<Node> &left);

    std::unique_ptr<Node> ifStmt();

    std::unique_ptr<Node> Conditionals();

    bool Parse();


    size_t sizeOfTree() {
      return compound.size();
    }

    std::vector<std::unique_ptr<Node> > compound;

  private:
    
    

    std::vector<LexerToken> token;
    size_t current;
    
};



#endif