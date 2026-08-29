#include "parser.hpp"
#include <cstddef>
#include "lexer/lexer.hpp"
#include "node.hpp"

namespace cx
{

    Parser::Parser(Lexer& lexer, Diagnostics& diag) : m_lex(lexer), m_diag(diag) {}

    void Parser::consume()
    {
        m_current = m_lex.nextNWToken();
    }

    void Parser::expect(TokenType type)
    {
        if (m_current.type == type)
        {
            consume();
            return;
        }

        m_diag.error(m_current.location, "expected '{}'", toString(type));
    }

    ProgramNode* Parser::Parse()
    {
        auto* root = new ProgramNode();

        while (m_current.type != TokenType::Eof)
        {
            root->statements.push_back(parseStatement());
        }

        return root;
    }

    Node* Parser::parseStatement()
    {
        switch (m_current.type)
        {
            case TokenType::If:
                return parseIfStatement();

            case TokenType::Print:
                return parsePrintStatement();

            case TokenType::Return:
                return parseReturnStatement();

            case TokenType::Func:
                return parseCallStatement();

            default:
                return parseExprStatement();
        }
    }

    Node* Parser::parseIfStatement()
    {
        auto* node = new IfNode(m_current, NodeType::IfStmt, nullptr, {}, {});

        // consume "if"
        consume();
        // parse condition
        node->condition = parseExpression();
        expect(TokenType::BracesOpen);

        // parse statements inside then-body
        while (m_current.type != TokenType::BracesClose) // what if there is never a closing braces
        {
            node->thenBody.push_back(parseStatement());
        }

        // consume '}'
        consume();

        // check for else
        if (m_current.type == TokenType::Else)
        {
            // consume "else"
            consume();
            expect(TokenType::BracesOpen);

            // parse statements inside else-body
            while (m_current.type != TokenType::BracesClose)
            {
                node->elseBody.push_back(parseStatement());
            }

            // consume '}'
            consume();
        }

        return node;
    }

    Node* Parser::parseExpression()
    {
        return parseAssignment();
    }

    Node* Parser::parseExprStatement()
    {
        auto expr = parseExpression();

        expect(TokenType::Semicolon);

        return expr;
    }

    Node* Parser::parseAssignment()
    {
        auto left = parseLogicalOr();

        while (m_current.type == TokenType::Assign)
        {
            auto* node = new BinaryNode(m_current, NodeType::BinaryExpr, left, nullptr);
            consume();
            node->right = parseAssignment();
            return node;
        }

        return left;
    }

    Node* Parser::parseLogicalOr()
    {
        auto left = parseLogicalAnd();
        while (m_current.type == TokenType::Or)
        {
            auto* node = new BinaryNode(m_current, NodeType::BinaryExpr, left, nullptr);
            consume();
            node->right = parseLogicalAnd();
            return node;
        }

        return left;
    }

    Node* Parser::parseLogicalAnd()
    {
        auto left = parseEquality();
        while (m_current.type == TokenType::And)
        {
            auto* node = new BinaryNode(m_current, NodeType::BinaryExpr, left, nullptr);
            consume();
            node->right = parseEquality();
            return node;
        }

        return left;
    }

    Node* Parser::parseEquality()
    {
        auto left = parseComparison();
        while (m_current.type == TokenType::Equal || m_current.type == TokenType::NotEqual)
        {
            auto* node = new BinaryNode(m_current, NodeType::BinaryExpr, left, nullptr);
            consume();
            node->right = parseComparison();
            return node;
        }

        return left;
    }

    Node* Parser::parseComparison()
    {
        auto left = parseTerm();
        while (m_current.type == TokenType::Less || m_current.type == TokenType::LessEqual || m_current.type == TokenType::Greater ||
               m_current.type == TokenType::GreaterEqual)
        {
            auto* node = new BinaryNode(m_current, NodeType::BinaryExpr, left, nullptr);
            consume();
            node->right = parseTerm();
            return node;
        }

        return left;
    }

    Node* Parser::parseTerm()
    {
        auto left = parseFactor();
        while (m_current.type == TokenType::Plus || m_current.type == TokenType::Minus)
        {
            auto* node = new BinaryNode(m_current, NodeType::BinaryExpr, left, nullptr);
            consume();

            node->right = parseFactor();
            return node;
        }

        return left;
    }

    Node* Parser::parseFactor()
    {
        auto left = parseUnary();

        while (m_current.type == TokenType::Divide || m_current.type == TokenType::Multiply || m_current.type == TokenType::Percent)
        {
            auto* node = new BinaryNode(m_current, NodeType::BinaryExpr, left, nullptr);
            consume();
            node->right = parseUnary();
            return node;
        }

        return left;
    }

    Node* Parser::parseUnary()
    {
        while (m_current.type == TokenType::Plus || m_current.type == TokenType::Minus || m_current.type == TokenType::Not)
        {
            auto* node = new UnaryNode(m_current, NodeType::UnaryExpr, nullptr);
            consume();
            node->operand = parseUnary();
            return node;
        }

        return parsePrimary();
    }

    Node* Parser::parsePrimary()
    {
        if (m_current.type == TokenType::Var)
        {
            auto* node = new BinaryNode(m_current, NodeType::Identifier, nullptr, nullptr);

            consume();
            return node;
        }

        if (m_current.type == TokenType::Bool)
        {
            auto* node = new BinaryNode(m_current, NodeType::Bool, nullptr, nullptr);

            consume();
            return node;
        }

        if (m_current.type == TokenType::String)
        {
            auto* node = new BinaryNode(m_current, NodeType::String, nullptr, nullptr);

            consume();
            return node;
        }

        if (m_current.type == TokenType::Float || m_current.type == TokenType::Int)
        {
            auto* node = new BinaryNode(m_current, NodeType::Number, nullptr, nullptr);

            consume();
            return node;
        }

        if (m_current.type == TokenType::ParenOpen)
        {
            consume();

            auto* expr = parseAssignment();

            expect(TokenType::ParenClose);

            return expr;
        }

        // Unexpected token: a primary expression was expected.
        m_diag.error(m_current.location, "expected expression");

        return nullptr;
    }

} // namespace cx

// Todo
//  i do not like the name advance
// calling advance every function ?? chnage that
// error handling
// I dont like the name parse this parse that
// change if to loops
// use expect

// Expression
//     ↓
// Assignment
//     ↓
// Conditional
//     ↓
// LogicalOr
//     ↓
// LogicalAnd
//     ↓
// Equality
//     ↓
// Comparison
//     ↓
// Term
//     ↓
// Factor
//     ↓
// Unary
//     ↓
// Primary