#pragma once

#include "lexer/lexer.hpp"
#include "node.hpp"

namespace cx
{

    class Parser
    {
    public:
        Parser(Lexer& lexer, Diagnostics& diag);

        ProgramNode* Parse();

    private:
        void         consume();
        void         expect(TokenType type);

        // statements
        Node*        parseStatement();
        Node*        parseIfStatement();
        Node*        parsePrintStatement();
        Node*        parseReturnStatement();
        Node*        parseExprStatement();
        Node*        parseCallStatement();

        // expressions
        Node*        parseExpression();
        Node*        parseAssignment();
        Node*        parseLogicalOr();
        Node*        parseLogicalAnd();
        Node*        parseEquality();
        Node*        parseComparison();
        Node*        parseTerm();
        Node*        parseFactor();
        Node*        parseUnary();
        Node*        parsePrimary();
        Node*        parseCall();

        Diagnostics& m_diag;
        Lexer&       m_lex;
        Token        m_current;
    };

} // namespace cx