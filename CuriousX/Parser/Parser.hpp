#pragma once

#include "Codegen.hpp"
#include <memory>
#include <string_view>
#include <vector>

class Lexer;
class ASTNode;
class TreeNode;
struct LexerToken;

class Parser
{
  public:
    explicit Parser(std::string_view data);

    bool parseTokens();

  private:
    // Parsing methods
    std::unique_ptr<ASTNode>  parseStatement(LexerToken& token);
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
    void advanceToken(LexerToken& token);
    bool expectNewlineOrEOF(const LexerToken& token) const;
    void advancePastNewlines(LexerToken& token);
    void handleUnexpectedToken(const LexerToken& token);

    // Member variables
    std::unique_ptr<Lexer>    m_lexer;
    std::unique_ptr<TreeNode> m_root;
    LexerToken                m_prevToken;
    WasmGen        m_sem;
};