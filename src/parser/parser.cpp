#include "parser.hpp"
#include "lexer/lexer.hpp"
#include "lexer/token.hpp"
#include "node.hpp"
#include "parser/arena.hpp"

namespace cx
{

    Parser::Parser(Lexer& lexer, Diagnostics& diag, Arena& arena) : m_lex(lexer), m_diag(diag), m_arena(arena)
    {
        consume();
    }

    ProgramNode* Parser::Parse()
    {
        std::vector<Node*> statements;
        while (m_current.type != TokenType::Eof)
        {
            const auto before = m_consumed;
            statements.push_back(parseStatement());

            // returned without advancing the token stream, maybe due to error
            if (m_consumed == before)
            {
                consume();
            }
        }

        return m_arena.create<ProgramNode>(Token{}, NodeKind::Program, ValueType::Unknown, m_arena.copyOf(statements));
    }

    void Parser::consume()
    {
        m_current = m_lex.nextToken();
        ++m_consumed;
    }

    //! match tokentype with current and consumes if true
    bool Parser::match(TokenType type)
    {
        if (m_current.type != type)
        {
            return false;
        }
        consume();
        return true;
    }

    //! match tokentype with current and consumes if true and record error if false
    bool Parser::expect(TokenType type)
    {
        if (m_current.type == type)
        {
            consume();
            return true;
        }
        m_diag.error(m_current.location, "expected '{}', found '{}'", describe(type), describe(m_current));
        return false;
    }

    //! for expected parenthesis and braces closing
    bool Parser::expectClosing(TokenType type, const Token& opener)
    {
        if (m_current.type == type)
        {
            consume();
            return true;
        }

        m_diag.error(
            m_current.location,
            "expected '{}' to match '{}' on line {}, found '{}'",
            describe(type),
            opener.value,
            opener.location.getLine(),
            m_current.value);
        return false;
    }

    //! For this parser we dont predict when there is an error we skip till we get the nearest token that can start a meaningful statement
    void Parser::synchronize()
    {
        while (m_current.type != TokenType::Eof)
        {
            switch (m_current.type)
            {
                case TokenType::Semicolon:
                    consume();
                    return;
                case TokenType::BracesClose: // let the enclosing block consume it
                case TokenType::If:
                case TokenType::While:
                case TokenType::Func:
                case TokenType::Return:
                case TokenType::Print:
                    return;
                default:
                    consume();
            }
        }
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
                return parseFuncDecl();

            case TokenType::While:
                return parseWhileLoop();
            case TokenType::BracesClose:
            {
                // to catch stray braces in blocks
                const Token stray = m_current;
                m_diag.error(stray.location, "unmatched '}}'");
                consume();
                return m_arena.create<Node>(stray, NodeKind::Error, ValueType::Unknown);
            }
            default:
                return parseExprStatement();
        }
    }

    Node* Parser::parseReturnStatement()
    {
        auto* node = m_arena.create<ReturnNode>(m_current, NodeKind::ReturnStmt, ValueType::Unknown, nullptr);
        consume();

        // return;
        if (m_current.type != TokenType::Semicolon)
        {
            node->expression = parseExpression();
        }

        if (!expect(TokenType::Semicolon))
        {
            synchronize();
        }
        return node;
    }

    Node* Parser::parsePrintStatement()
    {
        auto* node = m_arena.create<PrintNode>(m_current, NodeKind::Print, ValueType::Unknown, nullptr);
        consume();

        // this also handles "()"
        node->expression = parseExpression();

        if (!expect(TokenType::Semicolon))
        {
            synchronize();
        }
        return node;
    }

    bool Parser::parseBlock(std::vector<Node*>& out)
    {
        // get the "{" token
        const Token brace = m_current;
        if (!expect(TokenType::BracesOpen))
        {
            synchronize();
            return false;
        }

        while (m_current.type != TokenType::BracesClose && m_current.type != TokenType::Eof)
        {
            const auto before = m_consumed;
            out.push_back(parseStatement());

            // returned without advancing the token stream, maybe due to error
            if (m_consumed == before)
            {
                consume();
            }
        }

        expectClosing(TokenType::BracesClose, brace);
        return true;
    }

    Node* Parser::parseCall(const Token& prevtoken)
    {
        std::vector<Node*> arguments;

        // parse primary already consumed "("
        const Token        parenTok = m_current;
        consume();

        // call Expr to get the parameters
        if (m_current.type != TokenType::ParenClose)
        {
            do
            {
                // start parsing fro logicalor since we dont want to allow assignments as an arg
                arguments.push_back(parseLogicalOr());

            } while (match(TokenType::Comma));
        }

        expectClosing(TokenType::ParenClose, parenTok);

        auto callee = m_arena.create<Node>(prevtoken, NodeKind::Identifier, ValueType::Unknown);
        return m_arena.create<CallNode>(prevtoken, NodeKind::FuncCall, ValueType::Unknown, callee, m_arena.copyOf(arguments));
    }

    Node* Parser::parseFuncDecl()
    {
        // consume "func" keyword
        const Token funcTok = m_current;
        consume();

        // get and consume function name
        const Token nameTok = m_current;
        if (!expect(TokenType::Var))
        {
            synchronize();
            return m_arena.create<Node>(funcTok, NodeKind::Error, ValueType::Unknown);
        }
        auto               name = m_arena.create<Node>(nameTok, NodeKind::Identifier, ValueType::Unknown);

        std::vector<Node*> parameters;
        std::vector<Node*> body;

        // get parameters (...)
        const Token        parenTok = m_current;
        if (expect(TokenType::ParenOpen))
        {
            if (m_current.type != TokenType::ParenClose)
            {
                do
                {
                    if (m_current.type == TokenType::Var)
                    {
                        parameters.push_back(m_arena.create<Node>(m_current, NodeKind::Identifier, ValueType::Unknown));
                        consume();
                    }
                    else
                    {
                        if (m_current.type == TokenType::ParenClose || m_current.type == TokenType::Eof)
                        {
                            break;
                        }
                        m_diag.error(m_current.location, "expected parameter name, found '{}'", describe(m_current));
                        parameters.push_back(m_arena.create<Node>(m_current, NodeKind::Error, ValueType::Unknown));
                        consume();
                    }
                } while (match(TokenType::Comma));
            }
            expectClosing(TokenType::ParenClose, parenTok);
        }

        // function body {...}
        if (!parseBlock(body))
        {
            return m_arena.create<Node>(funcTok, NodeKind::Error, ValueType::Unknown);
        }

        return m_arena.create<FuncNode>(m_current, NodeKind::FuncDecl, ValueType::Unknown, name, m_arena.copyOf(parameters), m_arena.copyOf(body));
    }

    Node* Parser::parseWhileLoop()
    {
        std::vector<Node*> loop;

        // consume "while" keyword
        const Token        whileTok = m_current;
        consume();

        // parse condition "()"
        auto condition = parseExpression();

        // parse body {...}
        if (!parseBlock(loop))
        {
            return m_arena.create<Node>(whileTok, NodeKind::Error, ValueType::Unknown);
        }

        return m_arena.create<WhileNode>(m_current, NodeKind::WhileStmt, ValueType::Unknown, condition, m_arena.copyOf(loop));
    }

    Node* Parser::parseIfStatement()
    {
        // consume "if" keyword
        const Token ifTok = m_current;
        consume();

        // parse condition "()"
        auto               condition = parseExpression();

        std::vector<Node*> then;
        std::vector<Node*> nelse;

        if (!parseBlock(then))
        {
            return m_arena.create<Node>(ifTok, NodeKind::Error, ValueType::Unknown);
        }

        // check for else
        if (m_current.type == TokenType::Else)
        {
            // consume "else" keyword
            consume();

            // `else if (...) {...}`
            if (m_current.type == TokenType::If)
            {
                nelse.push_back(parseIfStatement());
            }
            else if (!parseBlock(nelse))
            {
                return m_arena.create<Node>(ifTok, NodeKind::Error, ValueType::Unknown);
            }
        }

        return m_arena.create<IfNode>(ifTok, NodeKind::IfStmt, ValueType::Unknown, condition, m_arena.copyOf(then), m_arena.copyOf(nelse));
    }

    Node* Parser::parseExprStatement()
    {
        auto expr = parseExpression();

        if (!expect(TokenType::Semicolon))
        {
            synchronize();
        }

        return expr;
    }

    Node* Parser::parseExpression()
    {
        return parseAssignment();
    }

    Node* Parser::parseAssignment()
    {
        auto* left = parseLogicalOr();

        if (m_current.type == TokenType::Assign)
        {
            // Get assignment "="
            const Token op = m_current;
            consume();

            Node* right = parseAssignment();

            // left node has to be a variable 
            if (left->kind != NodeKind::Identifier)
            {
                if (left->kind != NodeKind::Error)
                {
                    m_diag.error(op.location, "left side of an assignment must be a variable");
                }
                return m_arena.create<Node>(op, NodeKind::Error, ValueType::Unknown);
            }

            return m_arena.create<BinaryNode>(op, NodeKind::BinaryExpr, ValueType::Unknown, left, right);
        }

        return left;
    }

    Node* Parser::parseLogicalOr()
    {
        auto left = parseLogicalAnd();
        while (m_current.type == TokenType::Or)
        {
            auto* node = m_arena.create<BinaryNode>(m_current, NodeKind::BinaryExpr, ValueType::Unknown, left, nullptr);
            consume();
            node->right = parseLogicalAnd();
            left        = node;
        }

        return left;
    }

    Node* Parser::parseLogicalAnd()
    {
        auto left = parseEquality();
        while (m_current.type == TokenType::And)
        {
            auto* node = m_arena.create<BinaryNode>(m_current, NodeKind::BinaryExpr, ValueType::Unknown, left, nullptr);
            consume();
            node->right = parseEquality();
            left        = node;
        }

        return left;
    }

    Node* Parser::parseEquality()
    {
        auto left = parseComparison();
        while (m_current.type == TokenType::Equal || m_current.type == TokenType::NotEqual)
        {
            auto* node = m_arena.create<BinaryNode>(m_current, NodeKind::BinaryExpr, ValueType::Unknown, left, nullptr);
            consume();
            node->right = parseComparison();
            left        = node;
        }

        return left;
    }

    Node* Parser::parseComparison()
    {
        auto left = parseTerm();
        while (m_current.type == TokenType::Less || m_current.type == TokenType::LessEqual || m_current.type == TokenType::Greater ||
               m_current.type == TokenType::GreaterEqual)
        {
            auto* node = m_arena.create<BinaryNode>(m_current, NodeKind::BinaryExpr, ValueType::Unknown, left, nullptr);
            consume();
            node->right = parseTerm();
            left        = node;
        }

        return left;
    }

    Node* Parser::parseTerm()
    {
        auto left = parseFactor();
        while (m_current.type == TokenType::Plus || m_current.type == TokenType::Minus)
        {
            auto* node = m_arena.create<BinaryNode>(m_current, NodeKind::BinaryExpr, ValueType::Unknown, left, nullptr);
            consume();

            node->right = parseFactor();
            left        = node;
        }

        return left;
    }

    Node* Parser::parseFactor()
    {
        auto left = parseUnary();

        while (m_current.type == TokenType::Divide || m_current.type == TokenType::Multiply || m_current.type == TokenType::Percent)
        {
            auto* node = m_arena.create<BinaryNode>(m_current, NodeKind::BinaryExpr, ValueType::Unknown, left, nullptr);
            consume();
            node->right = parseUnary();
            left        = node;
        }

        return left;
    }

    Node* Parser::parseUnary()
    {
        // guard against deep nests
        if (m_depth >= kMaxDepth)
        {
            m_diag.error(m_current.location, "expression nests too deeply (limit {})", kMaxDepth);
            synchronize();
            return m_arena.create<Node>(m_current, NodeKind::Error, ValueType::Unknown);
        }
        const DepthGuard guard(m_depth);

        if (m_current.type == TokenType::Minus || m_current.type == TokenType::Not)
        {
            auto* node = m_arena.create<UnaryNode>(m_current, NodeKind::UnaryExpr, ValueType::Unknown, nullptr);
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
            const Token temp = m_current;
            consume();

            // check is this is a function call: by checking if it has "(" immediately after 
            if (m_current.type == TokenType::ParenOpen)
            {
                return parseCall(temp);
            }

            return m_arena.create<Node>(temp, NodeKind::Identifier, ValueType::Unknown);
        }

        if (m_current.type == TokenType::Bool)
        {
            auto* node = m_arena.create<Node>(m_current, NodeKind::Bool, ValueType::Unknown);

            consume();
            return node;
        }

        if (m_current.type == TokenType::String)
        {
            auto* node = m_arena.create<Node>(m_current, NodeKind::String, ValueType::Unknown);

            consume();
            return node;
        }

        if (m_current.type == TokenType::Float || m_current.type == TokenType::Int)
        {
            auto* node = m_arena.create<Node>(m_current, NodeKind::Number, ValueType::Unknown);

            consume();
            return node;
        }

        if (m_current.type == TokenType::ParenOpen)
        {
            const auto opener = m_current;
            consume();

            auto* expr = parseExpression();

            expectClosing(TokenType::ParenClose, opener);

            return expr;
        }

        // Unexpected token: a primary expression was expected.
        m_diag.error(m_current.location, "expected expression, found '{}'", describe(m_current));
        return m_arena.create<Node>(m_current, NodeKind::Error, ValueType::Unknown);
    }

} // namespace cx
