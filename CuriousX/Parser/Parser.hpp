#pragma once

#include "CompilerOutputParser.hpp"
#include <vector>

class Parser
{
  public:
    explicit Parser(std::string_view data) : lexer(std::make_unique<Lexer>(data))
    {
        LexerToken t;
        t.location = {0, 0};
        t.type = LexerTokenType::ProgramToken;
        t.value = "Program";
        root = ASTNodeFactory::createTreeNode({}, t);
    }

    bool ParseTokens();

    std::unique_ptr<ASTNode> Statement(LexerToken& token);

    std::unique_ptr<ASTNode> Factor(LexerToken& token);

    std::unique_ptr<ASTNode> Expression(LexerToken& token);
    std::unique_ptr<ASTNode> Term(LexerToken& token);

    std::unique_ptr<ASTNode> Assign(std::unique_ptr<ASTNode>& left, LexerToken& token);

    bool isValidFactorStart(LexerTokenType type);

    //   std::shared_ptr<Node> Expression();

    //   std::shared_ptr<Node> Term();

    //   std::shared_ptr<Node> Factor();

    //   std::shared_ptr<Node> Print();

    //   std::shared_ptr<Node> Assign(std::shared_ptr<Node> &left);

    //  const std::vector<std::shared_ptr<Node> > astRoot();

    //   bool Parse();

  private:
    std::unique_ptr<TreeNode> root;
    std::unique_ptr<Lexer> lexer;

    // std::shared_ptr<TreeNode> dummy_root;
};
