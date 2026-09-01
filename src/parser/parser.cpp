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
        //Todo no token for program node ?? why
        auto* root = m_arena.create<ProgramNode>(Token{}, NodeType::Program, std::vector<Node*>{});

        while (m_current.type != TokenType::Eof)
        {
            const auto before = m_consumed;
            root->statements.push_back(parseStatement());

            // no movement ??
            if (m_consumed == before)
            {
                consume();
            }
        }

        return root;
    }

    void Parser::consume()
    {
        m_current = m_lex.nextToken();
        ++m_consumed;
    }

    bool Parser::match(TokenType type)
    {
        if (m_current.type != type)
        {
            return false;
        }
        consume();
        return true;
    }

    // match but with error recorded
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

    // for expected parenthesis and braces closing
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

    // For this parser we dont predict when there is an error we skip till we get the nearest token that can start a meaningful statement
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
                const Token stray = m_current;
                m_diag.error(stray.location, "unmatched '}}'");
                consume();
                return  m_arena.create<Node>(stray, NodeType::Error);
            }

            default:
                return parseExprStatement();
        }
    }

    Node* Parser::parseReturnStatement()
    {
        auto* node =  m_arena.create<ReturnNode>(m_current, NodeType::ReturnStmt, nullptr);
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
        auto* node =  m_arena.create<PrintNode>(m_current, NodeType::Print, nullptr);
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

            // same backstop as Parse(): never loop without consuming
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
        auto* node           = m_arena.create<CallNode>(prevtoken, NodeType::FuncCall, nullptr, std::vector<Node*>{});
        node->callee         =  m_arena.create<Node>(prevtoken, NodeType::Identifier);

        // parse primary already consumed "("
        const Token parenTok = m_current;
        consume();

        // call Expr to get the parameters
        if (m_current.type != TokenType::ParenClose)
        {
            do
            {
                node->arguments.push_back(parseExpression());

            } while (match(TokenType::Comma));
        }

        expectClosing(TokenType::ParenClose, parenTok);

        return node;
    }

    Node* Parser::parseFuncDecl()
    {
        auto*       node    =  m_arena.create<FuncNode>(m_current, NodeType::FuncDecl, nullptr, std::vector<Node*>{}, std::vector<Node*>{});

        // consume "func"
        const Token funcTok = m_current;
        consume();

        // get and consume function name
        const Token nameTok = m_current;
        if (!expect(TokenType::Var))
        {
            synchronize();
            return  m_arena.create<Node>(funcTok, NodeType::Error);
        }
        node->name           =  m_arena.create<Node>(nameTok, NodeType::Identifier);

        // get parameters (...)
        const Token parenTok = m_current;
        if (expect(TokenType::ParenOpen))
        {
            if (m_current.type != TokenType::ParenClose)
            {
                do
                {
                    if (m_current.type == TokenType::Var)
                    {
                        node->parameters.push_back( m_arena.create<Node>(m_current, NodeType::Identifier));
                        consume();
                    }
                    else
                    {
                        if (m_current.type == TokenType::ParenClose || m_current.type == TokenType::Eof)
                        {
                            break;
                        }
                        m_diag.error(m_current.location, "expected parameter name, found '{}'", describe(m_current));
                        node->parameters.push_back( m_arena.create<Node>(m_current, NodeType::Error));
                        consume();
                    }
                } while (match(TokenType::Comma));
            }
            expectClosing(TokenType::ParenClose, parenTok);
        }

        // function body
        if (!parseBlock(node->body))
        {
            return  m_arena.create<Node>(funcTok, NodeType::Error);
        }

        return node;
    }

    Node* Parser::parseWhileLoop()
    {
        auto*       node     =  m_arena.create<WhileNode>(m_current, NodeType::WhileStmt, nullptr, std::vector<Node*>{});

        // consume "while"
        const Token whileTok = m_current;
        consume();

        // parse condition
        node->condition = parseExpression();

        if (!parseBlock(node->loop))
        {
            return  m_arena.create<Node>(whileTok, NodeType::Error);
        }

        return node;
    }

    Node* Parser::parseIfStatement()
    {
        auto*       node  =  m_arena.create<IfNode>(m_current, NodeType::IfStmt, nullptr, std::vector<Node*>{}, std::vector<Node*>{});

        // consume "if"
        const Token ifTok = m_current;
        consume();
        // parse condition
        node->condition = parseExpression();

        if (!parseBlock(node->then))
        {
            return  m_arena.create<Node>(ifTok, NodeType::Error);
        }

        // check for else
        if (m_current.type == TokenType::Else)
        {
            // consume "else"
            consume();

            // `else if (...) {...}`
            if (m_current.type == TokenType::If)
            {
                node->nelse.push_back(parseIfStatement());
            }
            else if (!parseBlock(node->nelse))
            {
                return  m_arena.create<Node>(ifTok, NodeType::Error);
            }
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

        if (!expect(TokenType::Semicolon))
        {
            synchronize();
        }

        return expr;
    }

    Node* Parser::parseAssignment()
    {
        auto* left = parseLogicalOr();

        if (m_current.type == TokenType::Assign)
        {
            const Token op = m_current;
            consume();
            Node* right = parseAssignment();

            if (left->type != NodeType::Identifier)
            {
                if (left->type != NodeType::Error)
                {
                    m_diag.error(op.location, "left side of an assignment must be a variable");
                }
                return m_arena.create<Node>(op, NodeType::Error);
            }

            return  m_arena.create<BinaryNode>(op, NodeType::BinaryExpr, left, right);
        }

        return left;
    }

    Node* Parser::parseLogicalOr()
    {
        auto left = parseLogicalAnd();
        while (m_current.type == TokenType::Or)
        {
            auto* node =  m_arena.create<BinaryNode>(m_current, NodeType::BinaryExpr, left, nullptr);
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
            auto* node =  m_arena.create<BinaryNode>(m_current, NodeType::BinaryExpr, left, nullptr);
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
            auto* node =  m_arena.create<BinaryNode>(m_current, NodeType::BinaryExpr, left, nullptr);
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
            auto* node =  m_arena.create<BinaryNode>(m_current, NodeType::BinaryExpr, left, nullptr);
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
            auto* node =  m_arena.create<BinaryNode>(m_current, NodeType::BinaryExpr, left, nullptr);
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
            auto* node = m_arena.create<BinaryNode>(m_current, NodeType::BinaryExpr, left, nullptr);
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
            return  m_arena.create<Node>(m_current, NodeType::Error);
        }
        const DepthGuard guard(m_depth);

        if (m_current.type == TokenType::Minus || m_current.type == TokenType::Not)
        {
            auto* node = m_arena.create<UnaryNode>(m_current, NodeType::UnaryExpr, nullptr);
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

            // function call ??
            if (m_current.type == TokenType::ParenOpen)
            {
                return parseCall(temp);
            }

            return m_arena.create<Node>(temp, NodeType::Identifier);
        }

        if (m_current.type == TokenType::Bool)
        {
            auto* node =  m_arena.create<Node>(m_current, NodeType::Bool);

            consume();
            return node;
        }

        if (m_current.type == TokenType::String)
        {
            auto* node =  m_arena.create<Node>(m_current, NodeType::String);

            consume();
            return node;
        }

        if (m_current.type == TokenType::Float || m_current.type == TokenType::Int)
        {
            auto* node =  m_arena.create<Node>(m_current, NodeType::Number);

            consume();
            return node;
        }

        if (m_current.type == TokenType::ParenOpen)
        {
            auto opener = m_current;
            consume();

            auto* expr = parseExpression();

            expectClosing(TokenType::ParenClose, opener);

            return expr;
        }

        // Unexpected token: a primary expression was expected.
        m_diag.error(m_current.location, "expected expression, found '{}'", describe(m_current));
        return m_arena.create<Node>(m_current, NodeType::Error);
    }

} // namespace cx
