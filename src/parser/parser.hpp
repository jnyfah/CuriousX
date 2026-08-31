#pragma once

#include "arena.hpp"
#include "lexer/lexer.hpp"
#include "node.hpp"

namespace cx
{

    //! Increments a depth counter for as long as it is alive.
    class DepthGuard
    {
    public:
        explicit DepthGuard(int& depth) : m_depth(depth)
        {
            ++m_depth;
        }
        ~DepthGuard()
        {
            --m_depth;
        }

        DepthGuard(const DepthGuard&)            = delete;
        DepthGuard& operator=(const DepthGuard&) = delete;

    private:
        int& m_depth;
    };

    class Parser
    {
    public:
        Parser(Lexer& lexer, Diagnostics& diag, size_t arenasize);

        ProgramNode* Parse();

    private:
        void                 consume();
        bool                 expect(TokenType type);
        void                 synchronize();
        bool                 expectClosing(TokenType type, const Token& opener);
        bool                 match(TokenType type);

        // statements
        Node*                parseStatement();
        Node*                parseIfStatement();
        Node*                parsePrintStatement();
        Node*                parseReturnStatement();
        Node*                parseExprStatement();
        Node*                parseFuncDecl();

        // Loops
        Node*                parseWhileLoop();

        // expressions
        Node*                parseExpression();
        Node*                parseAssignment();
        Node*                parseLogicalOr();
        Node*                parseLogicalAnd();
        Node*                parseEquality();
        Node*                parseComparison();
        Node*                parseTerm();
        Node*                parseFactor();
        Node*                parseUnary();
        Node*                parsePrimary();
        Node*                parseCall(const Token& prevtoken);

        bool                 parseBlock(std::vector<Node*>& out);

        static constexpr int kMaxDepth = 200;

        Lexer&               m_lex;
        Diagnostics&         m_diag;
        Token                m_current;
        int                  m_depth    = 0;
        int                  m_consumed = 0;
        Arena m_arena;
    };

} // namespace cx