#pragma once

#include "CompilerOutputParser.hpp"
#include "Semantic.hpp"
#include <vector>

class Parser
{
  public:
    explicit Parser(std::string_view data)
        : m_lexer(std::make_unique<Lexer>(data)),
          m_root(ASTNodeFactory::createTreeNode({},
                                                {"Program", {0, 0}, LexerTokenType::ProgramToken})),
          m_prevToken({"Program", {0, 0}, LexerTokenType::ProgramToken}), m_semantic()
    {
    }

    bool parseTokens();

    std::unique_ptr<ASTNode> parseStatement(LexerToken& token);
    std::unique_ptr<ASTNode> parseFactor(LexerToken& token);
    std::unique_ptr<ASTNode> parseExpression(LexerToken& token);
    std::unique_ptr<ASTNode> parseTerm(LexerToken& token);
    std::unique_ptr<ASTNode> parseConditional(LexerToken& token);
    std::unique_ptr<ASTNode> parseAssignment(std::unique_ptr<ASTNode>& left, LexerToken& token);
    std::unique_ptr<ASTNode> parseComparisonExpression(LexerToken& token);
    std::unique_ptr<TreeNode> parseBlock(LexerToken& token, LexerToken what);
    std::unique_ptr<ASTNode> parsePrintStatement(LexerToken& token);
    std::unique_ptr<ASTNode> parsePrintExpression(LexerToken& token);

  private:
    bool isValidFactorStart(LexerTokenType type);
    void advanceToken(LexerToken& token);
    bool expectNewlineOrEOF(const LexerToken& token) const;
    void advancePastNewlines(LexerToken& token);

    std::unique_ptr<TreeNode> m_root;
    std::unique_ptr<Lexer> m_lexer;
    LexerToken m_prevToken;
    Semantic m_semantic;
};