#pragma once

#include "CompilerOutputParser.hpp"
#include "Node.hpp"
#include <memory>
#include <string_view>
#include <vector>


class Parser
{
  public:
    explicit Parser(std::string_view data, CompilerOutput& output);

    std::unique_ptr<ASTNode> parseStatement(LexerToken& token);
    void                     advanceToken(LexerToken& token);
    bool                     expectNewlineOrEOF(const LexerToken& token) const;
    void                     advancePastNewlines(LexerToken& token);

  private:
    // Parsing methods
    std::unique_ptr<ASTNode>  parseExpression(LexerToken& token);
    std::unique_ptr<ASTNode>  parseTerm(LexerToken& token);
    std::unique_ptr<ASTNode>  parseFactor(LexerToken& token);
    std::unique_ptr<ASTNode>  parseConditional(LexerToken& token);
    std::unique_ptr<ASTNode>  parseAssignment(std::unique_ptr<ASTNode>& left, LexerToken& token);
    std::unique_ptr<ASTNode>  parseComparisonExpression(LexerToken& token);
    std::unique_ptr<TreeNode> parseBlock(LexerToken& token, LexerToken what);
    std::unique_ptr<ASTNode>  parsePrintStatement(LexerToken& token);
    std::unique_ptr<ASTNode>  parsePrintExpression(LexerToken& token);
    std::unique_ptr<ASTNode>  parsePrintTerm(LexerToken& token);

    // Helper methods
    bool isValidFactorStart(LexerTokenType type);
    void handleUnexpectedToken(const LexerToken& token);

    void addTokenToOutput(const LexerToken& token);
        // Member variables
        std::unique_ptr<Lexer> m_lexer;
    LexerToken      m_prevToken;
    CompilerOutput& m_output;
};