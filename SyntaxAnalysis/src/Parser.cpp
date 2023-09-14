#include "SyntaxAnalysis/include/Parser.hpp"
#include <iostream>


const std::vector<std::shared_ptr<Node> > Parser::astRoot() {
    return compound;
}
bool Parser::Parse()
{
    if (current >= token.size()) { return false; }

    while (current < token.size())
    {
        root = Expression();
        compound.push_back(root);
    }

    if (compound.empty()) { return false; }
    return true;
}



std::shared_ptr<Node> Parser::Assign(std::shared_ptr<Node> &left)
{
    LexerToken type = token[current];
    current++;

    std::shared_ptr<Node> right = Expression();

    return makeNode(left, right, type);;
}




/**
 * @ Expression is made of sum of Terms
 *
 * E -> T+E || T-E || T
 */

std::shared_ptr<Node> Parser::Expression()
{
    std::shared_ptr<Node> left = Term();

    while (true)
    {
        if (current >= token.size()) { return left; }

        if ((token[current].type == LexerTokenType::PlusToken) || (token[current].type == LexerTokenType::MinusToken))
        {

            LexerToken type = token[current];
            current++;

            if (current >= token.size() || !(token[current].type == LexerTokenType::VarToken || token[current].type == LexerTokenType::FloatToken
            || token[current].type == LexerTokenType::IntToken|| token[current].type == LexerTokenType::ParenOpen)) { throw Error("Unbalanced Expression, Missing or extra tokens at", left->type.location); }

            std::shared_ptr<Node> right = Term();
            left = makeNode(left, right, type);

        } else {
            return left;
        }
    }
}





/**
 * @ Term is a product of factors
 *
 * T -> F*T || F/T || F
 */

std::shared_ptr<Node> Parser::Term()
{
    std::shared_ptr<Node> left = Factor();

    if (current >= token.size()) { return left; }

    // Parse assign token
    if (token[current].type == LexerTokenType::AssignToken) { return Assign(left); }

    while (true)
    {
        if (current >= token.size()) { return left; }

        if ((token[current].type == LexerTokenType::MultiplyToken)
            || (token[current].type == LexerTokenType::DivideToken))
        {
            LexerToken type = token[current];
            current++;

            if (current >= token.size() || !(token[current].type == LexerTokenType::VarToken || token[current].type == LexerTokenType::FloatToken
            || token[current].type == LexerTokenType::IntToken || token[current].type == LexerTokenType::ParenOpen)) { throw Error("Unbalanced Expression, Missing or extra tokens at", left->type.location); }

            std::shared_ptr<Node> right = Factor();
            left = makeNode(left, right, type);

        } else {
            return left;
        }
    }
}




/**
 * @ Factor is a number, string or parenthesized sub expression
 *
 * F -> ID || Integer || E
 */
std::shared_ptr<Node> Parser::Factor()
{
    std::shared_ptr<Node> node = std::make_shared<Node>();

    if (current >= token.size()) { return node; }

    // To do variable name, if,
    if ((token[current].type == LexerTokenType::IntToken) || (token[current].type == LexerTokenType::FloatToken)
        || (token[current].type == LexerTokenType::VarToken))
    {
        node = makeLeaf(token[current]);
        current++;
        return node;
    }
    // if parenthesized sub expression
    else if (token[current].type == LexerTokenType::ParenOpen)
    {
        current++;
        std::shared_ptr<Node> left = Expression();

        if (token[current].type == LexerTokenType::ParenClose)
        {
            current++;
            return left;

        } else {
            throw Error("no closing braces", left->type.location);
        }

    } else if (token[current].type == LexerTokenType::PrintToken)
    {
        return Print();
    }

    return node;
}




/**
 * @ Print out a number, string, variable or even expression
 *
 * Print -> ID || Integer || E || String
 */

std::shared_ptr<Node> Parser::Print()
{
    std::shared_ptr<Node> left = std::make_shared<Node>();

    LexerToken type = token[current];
    current++;

    // check for parenthesis
    if (token[current].type != LexerTokenType::ParenOpen)
    { throw Error("no opening braces after print", token[current].location); }

    current++; left = Expression();


    // ensure parenthesis is closed
    if (token[current].type != LexerTokenType::ParenClose)
    { throw Error("no closing braces after print", token[current].location); }

    current++;

    return makeUnary(left, type);
}
