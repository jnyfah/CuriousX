#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>

#include "Error.hpp"
#include "Node.hpp"


class Parser
{
  public:
    explicit Parser(const std::vector<LexerToken>& token)
      : token(token), root(std::make_shared<Node>()), current(0)
    {}

    size_t sizeOfTree() {
      return compound.size();
    }


    

    std::shared_ptr<Node> Expression();

    std::shared_ptr<Node> Term();

    std::shared_ptr<Node> Factor();

    std::shared_ptr<Node> Print();

    std::shared_ptr<Node> Assign(std::shared_ptr<Node> &left);

   const std::vector<std::shared_ptr<Node> > astRoot();


    bool Parse();

    

  private:
    size_t current;
    std::shared_ptr<Node> root;
    std::vector<LexerToken> token;
    std::vector<std::shared_ptr<Node> > compound;
    
};



#endif