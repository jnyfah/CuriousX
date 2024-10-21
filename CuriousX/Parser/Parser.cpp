#include "Parser.hpp"
#include <memory>
#include <string>

Parser::Parser(std::string_view data, CompilerOutput& output)
    : m_lexer(std::make_unique<Lexer>(data))
    , m_prevToken({"Program", {0, 0}, LexerTokenType::ProgramToken})
    , m_output(output)

{
}

void Parser::addTokenToOutput(const LexerToken& token)
{
    m_output.getJson()["Lexer"].push_back(
        {{"type", toString(token.type)}, {"value", token.value}, {"location", token.location.toString()}});
}

    // Helper methods
    bool Parser::expectNewlineOrEOF(const LexerToken& token) const
    {
        return token.type == LexerTokenType::Newline || token.type == LexerTokenType::Eof ||
               token.type == LexerTokenType::CommentToken || m_prevToken.type == LexerTokenType::Newline;
    }

    void Parser::advancePastNewlines(LexerToken & token)
    {
        while (token.type == LexerTokenType::Newline || token.type == LexerTokenType::CommentToken)
            advanceToken(token);
    }

    void Parser::advanceToken(LexerToken & token)
    {
        m_prevToken = token;
        token       = m_lexer->nextNWToken();
        addTokenToOutput(token);
    }

    bool Parser::isValidFactorStart(LexerTokenType type)
    {
        return type == LexerTokenType::VarToken || type == LexerTokenType::FloatToken ||
               type == LexerTokenType::IntToken || type == LexerTokenType::StringToken ||
               type == LexerTokenType::BoolToken;
    }

    // Parsing methods
    std::unique_ptr<ASTNode> Parser::parseStatement(LexerToken & token)
    {
        switch (token.type)
        {
        case LexerTokenType::CommentToken:
        case LexerTokenType::Newline:
            advanceToken(token);
            return parseStatement(token);
        case LexerTokenType::IfToken:
            return parseConditional(token);
        case LexerTokenType::PrintToken:
            return parsePrintStatement(token);
        default:
            return parseExpression(token);
        }
    }

    std::unique_ptr<ASTNode> Parser::parseExpression(LexerToken & token)
    {
        auto left = parseTerm(token);
        while (token.type == LexerTokenType::PlusToken || token.type == LexerTokenType::MinusToken)
        {
            auto op = token;
            advanceToken(token);
            auto right = parseTerm(token);
            left       = ASTNodeFactory::createBinaryNode(std::move(left), std::move(right), op);
        }
        return left;
    }

    std::unique_ptr<ASTNode> Parser::parseTerm(LexerToken & token)
    {
        auto left = parseFactor(token);
        advanceToken(token);

        if (token.type == LexerTokenType::Eof)
            return left;

        if (token.type == LexerTokenType::AssignToken)
            return parseAssignment(left, token);

        while (token.type == LexerTokenType::MultiplyToken || token.type == LexerTokenType::DivideToken)
        {
            auto op = token;
            advanceToken(token);
            auto right = parseFactor(token);
            left       = ASTNodeFactory::createBinaryNode(std::move(left), std::move(right), op);
            advanceToken(token);
        }
        return left;
    }

    std::unique_ptr<ASTNode> Parser::parseAssignment(std::unique_ptr<ASTNode> & left, LexerToken & token)
    {
        if (left->token.type != LexerTokenType::VarToken)
            throw Error("Chained assignments are not allowed", left->token.location, ErrorType::SYNTAX);
        auto type = token;
        advanceToken(token);
        std::unique_ptr<ASTNode> right = parseExpression(token);
        return ASTNodeFactory::createBinaryNode(std::move(left), std::move(right), type);
    }

    std::unique_ptr<ASTNode> Parser::parseFactor(LexerToken & token)
    {
        if (isValidFactorStart(token.type))
        {
            return ASTNodeFactory::createBinaryNode(nullptr, nullptr, token);
        }
        else if (token.type == LexerTokenType::ParenOpen)
        {
            advanceToken(token);
            auto expr = parseExpression(token);
            if (token.type != LexerTokenType::ParenClose)
            {
                throw Error("Expected closing parenthesis", token.location, ErrorType::SYNTAX);
            }
            return expr;
        }

        handleUnexpectedToken(token);
        return nullptr;
    }

    void Parser::handleUnexpectedToken(const LexerToken& token)
    {
        switch (token.type)
        {
        case LexerTokenType::ElseToken:
            throw Error(
                "Unexpected 'else' keyword. 'else' must be preceded by 'if'", token.location, ErrorType::SYNTAX);
        case LexerTokenType::ParenClose:
            throw Error("Unexpected closing parenthesis ')'", token.location, ErrorType::SYNTAX);
        case LexerTokenType::Eof:
            throw Error("Unexpected end of file. Expression is incomplete", token.location, ErrorType::SYNTAX);
        case LexerTokenType::AssignToken:
            throw Error("Assignment is not allowed within print statement", token.location, ErrorType::SYNTAX);
        default:
            throw Error(
                "Unexpected token '" + std::string(token.value) + "' in factor", token.location, ErrorType::SYNTAX);
        }
    }

    std::unique_ptr<ASTNode> Parser::parseConditional(LexerToken & token)
    {
        if (m_prevToken.type != LexerTokenType::Newline && m_prevToken.type != LexerTokenType::ProgramToken)
        {
            throw Error("'if' statement cannot start a program and must start on a new line",
                        token.location,
                        ErrorType::SYNTAX);
        }

        // condition
        auto op = token;
        advanceToken(token);
        auto cond = parseComparisonExpression(token);

        // then block
        advanceToken(token);
        auto then = parseBlock(token, {"then", {0, 0}, LexerTokenType::ElseToken});

        // else block
        std::unique_ptr<TreeNode> elseBlock = nullptr;
        while (token.type == LexerTokenType::Newline)
            advanceToken(token);
        if (token.type == LexerTokenType::ElseToken)
        {
            advanceToken(token);
            elseBlock = parseBlock(token, {"Else", {0, 0}, LexerTokenType::ElseToken});
        }

        return ASTNodeFactory::createConditionalNode(std::move(cond), std::move(then), std::move(elseBlock), op);
    }

    std::unique_ptr<ASTNode> Parser::parseComparisonExpression(LexerToken & token)
    {
        if (token.type != LexerTokenType::ParenOpen)
            throw Error("Expected opening parenthesis", token.location, ErrorType::SYNTAX);

        advanceToken(token);
        auto left = parseExpression(token);

        if (token.type == LexerTokenType::GreaterToken || token.type == LexerTokenType::LessToken ||
            token.type == LexerTokenType::GreaterEqualToken || token.type == LexerTokenType::LessEqualToken ||
            token.type == LexerTokenType::EqualToken || token.type == LexerTokenType::NotEqualToken)
        {
            auto op = token;
            advanceToken(token);
            auto right = parseExpression(token);
            left       = ASTNodeFactory::createBinaryNode(std::move(left), std::move(right), op);
        }

        if (token.type != LexerTokenType::ParenClose)
            throw Error("Expected closing Braces", token.location, ErrorType::SYNTAX);
        return left;
    }

    std::unique_ptr<TreeNode> Parser::parseBlock(LexerToken & token, LexerToken what)
    {
        while (token.type == LexerTokenType::Newline)
            advanceToken(token);

        std::vector<std::unique_ptr<ASTNode>> statements;
        LexerToken                            blockToken = what;

        if (token.type != LexerTokenType::BracesOpen)
            throw Error("Expected opening braces for block", token.location, ErrorType::SYNTAX);

        advanceToken(token); // Consume '{'

        while (token.type != LexerTokenType::BracesClose)
        {
            while (token.type == LexerTokenType::Newline)
                advanceToken(token);

            if (token.type == LexerTokenType::BracesClose)
                break;

            auto statement = parseStatement(token);
            statements.push_back(std::move(statement));

            while (token.type == LexerTokenType::Newline)
                advanceToken(token);
        }

        if (token.type != LexerTokenType::BracesClose)
            throw Error("Expected closing braces at end of block", token.location, ErrorType::SYNTAX);

        advanceToken(token); // Consume '}'
        return ASTNodeFactory::createTreeNode(std::move(statements), blockToken);
    }

    std::unique_ptr<ASTNode> Parser::parsePrintStatement(LexerToken & token)
    {
        auto printToken = token;
        advanceToken(token); // Consume 'print' token

        if (token.type != LexerTokenType::ParenOpen)
            throw Error("Expected opening parenthesis after 'print'", token.location, ErrorType::SYNTAX);

        advanceToken(token); // Consume '('

        auto expression = parsePrintExpression(token);

        if (token.type != LexerTokenType::ParenClose)
            throw Error("Expected closing parenthesis after print expression", token.location, ErrorType::SYNTAX);

        advanceToken(token); // Consume ')'

        std::vector<std::unique_ptr<ASTNode>> children;
        children.emplace_back(std::move(expression));

        return ASTNodeFactory::createTreeNode(std::move(children), printToken);
    }

    std::unique_ptr<ASTNode> Parser::parsePrintExpression(LexerToken & token)
    {
        auto left = parsePrintTerm(token);

        while (token.type == LexerTokenType::PlusToken || token.type == LexerTokenType::MinusToken ||
               token.type == LexerTokenType::GreaterToken || token.type == LexerTokenType::LessToken ||
               token.type == LexerTokenType::GreaterEqualToken || token.type == LexerTokenType::LessEqualToken ||
               token.type == LexerTokenType::EqualToken || token.type == LexerTokenType::NotEqualToken)
        {
            auto op = token;
            advanceToken(token);
            auto right = parseTerm(token);
            left       = ASTNodeFactory::createBinaryNode(std::move(left), std::move(right), op);
        }

        return left;
    }

    std::unique_ptr<ASTNode> Parser::parsePrintTerm(LexerToken & token)
    {
        auto left = parseFactor(token);
        advanceToken(token);

        if (token.type == LexerTokenType::AssignToken)
            throw Error("Assignment is not allowed within print statement", token.location, ErrorType::SYNTAX);

        while (token.type == LexerTokenType::MultiplyToken || token.type == LexerTokenType::DivideToken)
        {
            auto op = token;
            advanceToken(token);
            auto right = parseFactor(token);
            left       = ASTNodeFactory::createBinaryNode(std::move(left), std::move(right), op);
        }

        return left;
    }
