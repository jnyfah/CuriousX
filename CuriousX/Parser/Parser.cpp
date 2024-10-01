#include "Parser.hpp"

bool Parser::parseTokens()
{
    LexerToken token;
    advanceToken(token);

    while (token.type != LexerTokenType::Eof)
    {
        if (auto node = parseStatement(token))
        {
            m_root->children.push_back(std::move(node));
        }
        if (!expectNewlineOrEOF(token))
        {
            throw Error("Expected new line or end of file at ", token.location);
        }
        advancePastNewlines(token);
    }
    CompilerOutputParser::getInstance().setASTOutput(m_root);
    return !m_root->children.empty();
}

bool Parser::expectNewlineOrEOF(const LexerToken& token) const
{
    return token.type == LexerTokenType::Newline || token.type == LexerTokenType::Eof ||
           token.type == LexerTokenType::CommentToken ||
           m_prevToken.type == LexerTokenType::Newline;
}

void Parser::advancePastNewlines(LexerToken& token)
{
    while (token.type == LexerTokenType::Newline || token.type == LexerTokenType::CommentToken)
        advanceToken(token);
}

std::unique_ptr<ASTNode> Parser::parseStatement(LexerToken& token)
{
    std::shared_ptr<ASTNode> Node;

    switch (token.type)
    {
        // edge case if comment is the last line
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

std::unique_ptr<ASTNode> Parser::parseAssignment(std::unique_ptr<ASTNode>& left, LexerToken& token)
{
    auto type = token;
    advanceToken(token);
    std::unique_ptr<ASTNode> right = parseExpression(token);
    return ASTNodeFactory::createBinaryNode(std::move(left), std::move(right), type);
}

bool Parser::isValidFactorStart(LexerTokenType type)
{
    return type == LexerTokenType::VarToken || type == LexerTokenType::FloatToken ||
           type == LexerTokenType::IntToken || type == LexerTokenType::StringToken ||
           type == LexerTokenType::BoolToken;
}

/**
 * @ parseExpression is made of sum of Terms
 *
 * E -> T+E || T-E || T
 */
std::unique_ptr<ASTNode> Parser::parseExpression(LexerToken& token)
{
    auto left = parseTerm(token);
    while (token.type == LexerTokenType::PlusToken || token.type == LexerTokenType::MinusToken)
    {
        auto op = token;
        advanceToken(token);
        auto right = parseTerm(token);
        left = ASTNodeFactory::createBinaryNode(std::move(left), std::move(right), op);
    }
    return left;
}

/**
 * @ parseTerm is a product of factors
 *
 * T -> F*T || F/T || F
 */
std::unique_ptr<ASTNode> Parser::parseTerm(LexerToken& token)
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
        left = ASTNodeFactory::createBinaryNode(std::move(left), std::move(right), op);
        advanceToken(token);
    }
    return left;
}

/**
 * @ parseFactor is a number, string or parenthesized sub expression
 *
 * F -> ID || Integer || E
 */
std::unique_ptr<ASTNode> Parser::parseFactor(LexerToken& token)
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
            throw Error("Expected closing parenthesis at ", token.location);
        }
        return expr;
    }

    if (token.type == LexerTokenType::ElseToken)
        throw Error("Expected if before Else at ", token.location);

    throw Error("Unexpected token at ", token.location);
}

std::unique_ptr<ASTNode> Parser::parseConditional(LexerToken& token)
{
    if (m_prevToken.type != LexerTokenType::Newline &&
        m_prevToken.type != LexerTokenType::ProgramToken)
    {
        throw Error("'if' statement must start on a new line at ", token.location);
    }

    auto op = token;

    // condition
    advanceToken(token);
    auto cond = parseComparisonExpression(token);

    // then block
    advanceToken(token);
    auto then = parseBlock(token);

    // else
    std::unique_ptr<ASTNode> elseBlock = nullptr;
    while (token.type == LexerTokenType::Newline)
        advanceToken(token);
    if (token.type == LexerTokenType::ElseToken)
    {
        advanceToken(token);
        elseBlock = parseBlock(token);
    }

    return ASTNodeFactory::createConditionalNode(std::move(cond), std::move(then),
                                                 std::move(elseBlock), op);
}

std::unique_ptr<ASTNode> Parser::parseBlock(LexerToken& token)
{
    while (token.type == LexerTokenType::Newline)
        advanceToken(token);

    if (token.type != LexerTokenType::BracesOpen)
        throw Error("Expected opening braces after condition at ", token.location);

    advanceToken(token); // Consume '{'
    auto block = parseStatement(token);

    while (token.type == LexerTokenType::Newline)
        advanceToken(token);

    if (token.type != LexerTokenType::BracesClose)
        throw Error("Expected closing braces after condition at ", token.location);
    advanceToken(token);

    return block;
}

std::unique_ptr<ASTNode> Parser::parseComparisonExpression(LexerToken& token)
{
    if (token.type != LexerTokenType::ParenOpen)
        throw Error("Expected opening parenthesis at ", token.location);

    advanceToken(token);

    auto left = parseExpression(token);

    if (token.type == LexerTokenType::GreaterToken || token.type == LexerTokenType::LessToken ||
        token.type == LexerTokenType::GreaterEqualToken ||
        token.type == LexerTokenType::LessEqualToken || token.type == LexerTokenType::EqualToken ||
        token.type == LexerTokenType::NotEqualToken)
    {
        auto op = token;
        advanceToken(token);
        auto right = parseExpression(token);
        left = ASTNodeFactory::createBinaryNode(std::move(left), std::move(right), op);
    }

    if (token.type != LexerTokenType::ParenClose)
        throw Error("Expected closing Braces at ", token.location);
    return left;
}

void Parser::advanceToken(LexerToken& token)
{
    m_prevToken = token;
    token = m_lexer->nextNWToken();
    CompilerOutputParser::getInstance().SetLexerOutput(token);
}
/**
 * @ Print out a number, string, variable or even expression
 *
 * Print -> ID || Integer || E || String
 */

std::unique_ptr<ASTNode> Parser::parsePrintStatement(LexerToken& token)
{
    auto printToken = token;
    advanceToken(token);
    auto expression = parseExpression(token);

    std::vector<std::unique_ptr<ASTNode>> children;
    children.push_back(std::move(expression));

    return ASTNodeFactory::createTreeNode(std::move(children), printToken);
}
