#include "Parser.hpp"

bool Parser::ParseTokens()
{
    // Error handling implementation
    // handle else
    // print
    // cleaning and adding comments
    // expecting newline before ifs
    // handling comments at the end of code

    LexerToken token;
    do
    {
        advanceToken(token);

        if (token.type != LexerTokenType::Eof)
        {
            auto node = Statement(token);
            if (node)
            {
                root->children.push_back(std::move(node));
            }
        }
    } while (token.type != LexerTokenType::Eof);

    CompilerOutputParser::getInstance().setASTOutput(root);

    return !root->children.empty();
}

std::unique_ptr<ASTNode> Parser::Statement(LexerToken& token)
{
    std::shared_ptr<ASTNode> Node;

    switch (token.type)
    {
        // edge case if comment is the last line
    case LexerTokenType::CommentToken:
    case LexerTokenType::Newline:
        token = lexer->nextNWToken();
        CompilerOutputParser::getInstance().SetLexerOutput(token);
        if (token.type == LexerTokenType::Newline)
        {
            token = lexer->nextNWToken();
            // might remove writing comments
            CompilerOutputParser::getInstance().SetLexerOutput(token);
        }
        return Statement(token);
    case LexerTokenType::IfToken:
        return Conditional(token);

    default:
        return Expression(token);
    }
}

std::unique_ptr<ASTNode> Parser::Assign(std::unique_ptr<ASTNode>& left, LexerToken& token)
{
    auto type = token;
    advanceToken(token);

    std::unique_ptr<ASTNode> right = Expression(token);
    return ASTNodeFactory::createBinaryNode(std::move(left), std::move(right), type);
}

// Helper function to check if a token can start a factor
bool Parser::isValidFactorStart(LexerTokenType type)
{
    return type == LexerTokenType::VarToken || type == LexerTokenType::FloatToken ||
           type == LexerTokenType::IntToken || type == LexerTokenType::StringToken ||
           type == LexerTokenType::BoolToken;
}

/**
 * @ Expression is made of sum of Terms
 *
 * E -> T+E || T-E || T
 */
std::unique_ptr<ASTNode> Parser::Expression(LexerToken& token)
{
    auto left = Term(token);
    while (token.type == LexerTokenType::PlusToken || token.type == LexerTokenType::MinusToken)
    {
        auto op = token;
        advanceToken(token);
        auto right = Term(token);
        left = ASTNodeFactory::createBinaryNode(std::move(left), std::move(right), op);
    }
    return left;
}

/**
 * @ Term is a product of factors
 *
 * T -> F*T || F/T || F
 */

std::unique_ptr<ASTNode> Parser::Term(LexerToken& token)
{
    auto left = Factor(token);
    advanceToken(token);

    if (token.type == LexerTokenType::Eof)
        return left;

    if (token.type == LexerTokenType::AssignToken)
        return Assign(left, token);

    while (token.type == LexerTokenType::MultiplyToken || token.type == LexerTokenType::DivideToken)
    {
        auto op = token;
        advanceToken(token);
        auto right = Factor(token);
        left = ASTNodeFactory::createBinaryNode(std::move(left), std::move(right), op);
        advanceToken(token);
    }
    return left;
}

/**
 * @ Factor is a number, string or parenthesized sub expression
 *
 * F -> ID || Integer || E
 */
std::unique_ptr<ASTNode> Parser::Factor(LexerToken& token)
{
    if (isValidFactorStart(token.type))
    {
        return ASTNodeFactory::createBinaryNode(nullptr, nullptr, token);
    }
    else if (token.type == LexerTokenType::ParenOpen)
    {
        advanceToken(token);
        auto expr = Expression(token);
        if (token.type != LexerTokenType::ParenClose)
        {
            throw Error("Expected closing parenthesis", token.location);
        }
        return expr;
    }
    throw Error("Unexpected token in factor", token.location);
}

std::unique_ptr<ASTNode> Parser::Conditional(LexerToken& token)
{
    auto op = token;
    CompilerOutputParser::getInstance().SetLexerOutput(token); // if

    // condition
    advanceToken(token);
    auto cond = ComparisonExpression(token);

    // then block
    advanceToken(token);
    while (token.type == LexerTokenType::Newline)
        advanceToken(token);
    if (token.type != LexerTokenType::BracesOpen)
        throw Error("Expected opening Braces", token.location);
    advanceToken(token);
    auto then = Statement(token);
    while (token.type == LexerTokenType::Newline)
        advanceToken(token);
    if (token.type != LexerTokenType::BracesClose)
        throw Error("Expected closing Braces", token.location);

    return ASTNodeFactory::createConditionalNode(std::move(cond), std::move(then), nullptr, op);
}

std::unique_ptr<ASTNode> Parser::ComparisonExpression(LexerToken& token)
{
    if (token.type != LexerTokenType::ParenOpen)
        throw Error("Expected opening parenthesis", token.location);

    advanceToken(token);

    auto left = Expression(token);

    if (token.type == LexerTokenType::GreaterToken || token.type == LexerTokenType::LessToken ||
        token.type == LexerTokenType::GreaterEqualToken ||
        token.type == LexerTokenType::LessEqualToken || token.type == LexerTokenType::EqualToken ||
        token.type == LexerTokenType::NotEqualToken)
    {
        auto op = token;
        advanceToken(token);
        auto right = Expression(token);
        left = ASTNodeFactory::createBinaryNode(std::move(left), std::move(right), op);
    }

    if (token.type != LexerTokenType::ParenClose)
        throw Error("Expected closing Braces", token.location);
    return left;
}

void Parser::advanceToken(LexerToken& token)
{
    token = lexer->nextNWToken();
    CompilerOutputParser::getInstance().SetLexerOutput(token);
}
// /**
//  * @ Print out a number, string, variable or even expression
//  *
//  * Print -> ID || Integer || E || String
//  */

// std::shared_ptr<Node> Parser::Print()
// {
//     std::shared_ptr<Node> left = std::make_shared<Node>();

//     LexerToken type = token[current];
//     current++;

//     // check for parenthesis
//     if (token[current].type != LexerTokenType::ParenOpen)
//     { throw Error("no opening braces after print", token[current].location); }

//     current++; left = Expression();

//     // ensure parenthesis is closed
//     if (token[current].type != LexerTokenType::ParenClose)
//     { throw Error("no closing braces after print", token[current].location); }

//     current++;

//     return makeUnary(left, type);
// }
