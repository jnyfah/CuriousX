#include "Parser.hpp"

bool Parser::ParseTokens()
{
    // Error handling implementation 
    // 

    LexerToken token;
    do
    {
        token = lexer->nextNWToken();
        CompilerOutputParser::getInstance().SetLexerOutput(token);

        if (token.type != LexerTokenType::Eof)
        {
            auto node = Statement(token);
            if (node)
            {
                root->children.push_back(std::move(node));
            }
        }
    } while (token.type != LexerTokenType::Eof);

    return !root->children.empty();
}

std::unique_ptr<ASTNode> Parser::Statement(LexerToken& token)
{
    std::shared_ptr<ASTNode>  Node;

    switch (token.type)
    {
    case LexerTokenType::CommentToken:
    case LexerTokenType::Newline:
        token = lexer->nextNWToken();
        CompilerOutputParser::getInstance().SetLexerOutput(token);
        if (token.type == LexerTokenType::Newline)
        {
            token = lexer->nextNWToken();
            // might removing writing comments
            CompilerOutputParser::getInstance().SetLexerOutput(token);
        }
        return Statement(token);

    default:
        return Expression(token);
        break;
    }
}

std::unique_ptr<ASTNode> Parser::Assign(std::unique_ptr<ASTNode>& left, LexerToken& token)
{
    auto type = token;
    token = lexer->nextNWToken();
    CompilerOutputParser::getInstance().SetLexerOutput(token);

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

std::unique_ptr<ASTNode> Parser::Expression(LexerToken& token)
{
    auto left = Term(token);
    while (token.type == LexerTokenType::PlusToken || token.type == LexerTokenType::MinusToken)
    {
        auto op = token;
        token = lexer->nextNWToken();
        CompilerOutputParser::getInstance().SetLexerOutput(token);
        auto right = Term(token);
        left = ASTNodeFactory::createBinaryNode(std::move(left), std::move(right), op);
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::Term(LexerToken& token)
{
    auto left = Factor(token);
    token = lexer->nextNWToken();
    CompilerOutputParser::getInstance().SetLexerOutput(token);

    if (token.type == LexerTokenType::Eof)
        return left;

    if (token.type == LexerTokenType::AssignToken)
        return Assign(left, token);

    while (token.type == LexerTokenType::MultiplyToken || token.type == LexerTokenType::DivideToken)
    {
        auto op = token;
        token = lexer->nextNWToken();
        CompilerOutputParser::getInstance().SetLexerOutput(token);
        auto right = Factor(token);
        left = ASTNodeFactory::createBinaryNode(std::move(left), std::move(right), op);
        token = lexer->nextNWToken();
        CompilerOutputParser::getInstance().SetLexerOutput(token);
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::Factor(LexerToken& token)
{
    if (isValidFactorStart(token.type))
    {
        return ASTNodeFactory::createBinaryNode(nullptr, nullptr, token);
    }
    else if (token.type == LexerTokenType::ParenOpen)
    {
        token = lexer->nextNWToken();
        CompilerOutputParser::getInstance().SetLexerOutput(token);
        auto expr = Expression(token);
        if (token.type != LexerTokenType::ParenClose)
        {
            throw Error("Expected closing parenthesis", token.location);
        }
        return expr;
    }
    throw Error("Unexpected token in factor", token.location);
}

// const std::vector<std::shared_ptr<Node> > Parser::astRoot() {
//     return compound;
// }
// bool Parser::Parse()
// {
//     if (current >= token.size()) { return false; }

//     while (current < token.size())
//     {
//         root = Expression();
//         compound.push_back(root);
//     }

//     if (compound.empty()) { return false; }
//     return true;
// }

// std::shared_ptr<Node> Parser::Assign(std::shared_ptr<Node> &left)
// {
//     LexerToken type = token[current];
//     current++;

//     std::shared_ptr<Node> right = Expression();

//     return makeNode(left, right, type);;
// }

// /**
//  * @ Expression is made of sum of Terms
//  *
//  * E -> T+E || T-E || T
//  */

// std::shared_ptr<Node> Parser::Expression()
// {
//     std::shared_ptr<Node> left = Term();

//     while (true)
//     {
//         if (current >= token.size()) { return left; }

//         if ((token[current].type == LexerTokenType::PlusToken) || (token[current].type ==
//         LexerTokenType::MinusToken))
//         {

//             LexerToken type = token[current];
//             current++;

//             if (current >= token.size() || !(token[current].type == LexerTokenType::VarToken ||
//             token[current].type == LexerTokenType::FloatToken
//             || token[current].type == LexerTokenType::IntToken|| token[current].type ==
//             LexerTokenType::ParenOpen)) { throw Error("Unbalanced Expression, Missing or extra
//             tokens at", left->type.location); }

//             std::shared_ptr<Node> right = Term();
//             left = makeNode(left, right, type);

//         } else {
//             return left;
//         }
//     }
// }

// /**
//  * @ Term is a product of factors
//  *
//  * T -> F*T || F/T || F
//  */

// std::shared_ptr<Node> Parser::Term()
// {
//     std::shared_ptr<Node> left = Factor();

//     if (current >= token.size()) { return left; }

//     // Parse assign token
//     if (token[current].type == LexerTokenType::AssignToken) { return Assign(left); }

//     while (true)
//     {
//         if (current >= token.size()) { return left; }

//         if ((token[current].type == LexerTokenType::MultiplyToken)
//             || (token[current].type == LexerTokenType::DivideToken))
//         {
//             LexerToken type = token[current];
//             current++;

//             if (current >= token.size() || !(token[current].type == LexerTokenType::VarToken ||
//             token[current].type == LexerTokenType::FloatToken
//             || token[current].type == LexerTokenType::IntToken || token[current].type ==
//             LexerTokenType::ParenOpen)) { throw Error("Unbalanced Expression, Missing or extra
//             tokens at", left->type.location); }

//             std::shared_ptr<Node> right = Factor();
//             left = makeNode(left, right, type);

//         } else {
//             return left;
//         }
//     }
// }

// /**
//  * @ Factor is a number, string or parenthesized sub expression
//  *
//  * F -> ID || Integer || E
//  */
// void Parser::Factor(const LexerToken& token)
// {
//     auto node = ASTNodeFactory::createBinaryNode(nullptr, nullptr, token);
//     std::shared_ptr<Node> node = std::make_shared<Node>();

//     if (current >= token.size()) { return node; }

//     // To do variable name, if,
//     if ((token[current].type == LexerTokenType::IntToken) || (token[current].type ==
//     LexerTokenType::FloatToken)
//         || (token[current].type == LexerTokenType::VarToken))
//     {
//         node = makeLeaf(token[current]);
//         current++;
//         return node;
//     }
//     // if parenthesized sub expression
//     else if (token[current].type == LexerTokenType::ParenOpen)
//     {
//         current++;
//         std::shared_ptr<Node> left = Expression();

//         if (token[current].type == LexerTokenType::ParenClose)
//         {
//             current++;
//             return left;

//         } else {
//             throw Error("no closing braces", left->type.location);
//         }

//     } else if (token[current].type == LexerTokenType::PrintToken)
//     {
//         return Print();
//     }

//     return node;
//}

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
// switch (token.type)
// {

//     // factor
// case LexerTokenType::FloatToken:
// case LexerTokenType::IntToken:
// case LexerTokenType::VarToken:
// case LexerTokenType::StringToken:
// case LexerTokenType::BoolToken:
// case LexerTokenType::ParenOpen:

// case LexerTokenType::ParenClose:

// case LexerTokenType::BracesOpen:

// case LexerTokenType::BracesClose:

// case LexerTokenType::IfToken:

// case LexerTokenType::ElseToken:

// case LexerTokenType::PlusToken:
// case LexerTokenType::MinusToken:

// case LexerTokenType::DivideToken:
// case LexerTokenType::MultiplyToken:

// case LexerTokenType::AssignToken:

// case LexerTokenType::PrintToken:

// case LexerTokenType::EqualToken:

// case LexerTokenType::NotEqualToken:
// case LexerTokenType::LessEqualToken:
// case LexerTokenType::LessToken:
// case LexerTokenType::GreaterEqualToken:
// case LexerTokenType::GreaterToken:

//     // do nothing
// case LexerTokenType::CommentToken:
// case LexerTokenType::Space:
// case LexerTokenType::Tab:
// case LexerTokenType::Newline:
// case LexerTokenType::Eof:

// default:
//     break;
// }