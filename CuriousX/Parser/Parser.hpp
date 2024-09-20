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

    std::unique_ptr<ASTNode> Conditional(LexerToken& token);

    std::unique_ptr<ASTNode> Assign(std::unique_ptr<ASTNode>& left, LexerToken& token);

    std::unique_ptr<ASTNode> ComparisonExpression(LexerToken& token);

    bool isValidFactorStart(LexerTokenType type);

    void advanceToken(LexerToken& token);

  private:
    std::unique_ptr<TreeNode> root;
    std::unique_ptr<Lexer> lexer;
};
