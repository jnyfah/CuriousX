#include "SyntaxAnalysis/include/Parser.hpp"
#include <iostream>



bool Parser::Parse()
{
    if (current >= token.size()) { return false; }

    while (current < token.size())
    {
        root = Expression();
        compound.push_back(std::move(root));
    }

    if (compound.empty()) { return false; }
    return true;
}



std::unique_ptr<Node> Parser::Assign(std::unique_ptr<Node> &left)
{
    LexerToken type = token[current];
    current++;

    std::unique_ptr<Node> right = Expression();

    return makeNode(std::move(left), std::move(right), type);;
}




/**
 * @ Expression is made of sum of Terms
 *
 * E -> T+E || T-E || T
 */

std::unique_ptr<Node> Parser::Expression()
{
    if (token[current].type == LexerTokenType::IfToken) { return Conditionals(); }
    std::unique_ptr<Node> left = Term();

    while (true)
    {
        if (current >= token.size()) { return left; }

        if ((token[current].type == LexerTokenType::PlusToken) || (token[current].type == LexerTokenType::MinusToken)
            || (token[current].type == LexerTokenType::GreaterThanToken) || (token[current].type == LexerTokenType::GreaterEqualToken)
            || (token[current].type == LexerTokenType::LessThanToken) || (token[current].type == LexerTokenType::LessEqualToken) 
            || (token[current].type == LexerTokenType::EqualToken) || (token[current].type == LexerTokenType::NotEqualToken))
        {

            LexerToken type = token[current];
            current++;

            if (current >= token.size()) { return left; }

            std::unique_ptr<Node> right = Term();
            left = makeNode(std::move(left), std::move(right), type);

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

std::unique_ptr<Node> Parser::Term()
{
    std::unique_ptr<Node> left = Factor();

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

            if (current >= token.size()) { return left; }

            std::unique_ptr<Node> right = Factor();
            left = makeNode(std::move(left), std::move(right), type);

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
std::unique_ptr<Node> Parser::Factor()
{
    std::unique_ptr<Node> node = std::make_unique<Node>();

    if (current >= token.size()) { return node; }

    // To do variable name, if,
    if ((token[current].type == LexerTokenType::IntToken) || (token[current].type == LexerTokenType::FloatToken)
        || (token[current].type == LexerTokenType::VarToken) || (token[current].type == LexerTokenType::StringToken))
    {
        node = makeLeaf(token[current]);
        current++;
        return node;
    }
    // if parenthesized sub expression
    else if (token[current].type == LexerTokenType::ParenOpen)
    {
        current++;
        std::unique_ptr<Node> left = Expression();

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
    }else if (token[current].type != LexerTokenType::BraceClose)
    {
        throw Error("no closing braces", token[current].location);
    }

    return node;
}




/**
 * @ Print out a number, string, variable or even expression
 *
 * Print -> ID || Integer || E || String
 */

std::unique_ptr<Node> Parser::Print()
{
    std::unique_ptr<Node> left = std::make_unique<Node>();

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

    return makeUnary(std::move(left), type);
}



std::unique_ptr<Node> Parser::Conditionals()
{
    std::unique_ptr<Node> left = std::make_unique<Node>();
    std::unique_ptr<Node> right = std::make_unique<Node>();

    LexerToken type = token[current];
    current++;

    // check for parenthesis
    if (token[current].type == LexerTokenType::ParenOpen)
    { 
        current++;
        left = Expression();

        // ensure parenthesis is closed
        if (token[current].type != LexerTokenType::ParenClose)
        { throw Error("no closing braces after if", token[current].location); }

    } else {
        throw Error("no opening braces before if", token[current].location);
    }
    
    current++;
    right = ifStmt();

    return makeNode(std::move(left), std::move(right), type);
}



std::unique_ptr<Node> Parser::ifStmt() {

    std::unique_ptr<Node> node = std::make_unique<Node>();

    if (token[current].type == LexerTokenType::BraceOpen)
    { 
        current++;
        node = Expression();

        if (token[current].type == LexerTokenType::BraceClose)
        { 
            current++; 
        }else {
            throw Error("no opening braces after paren", token[current].location);
        }

    } else {
      throw Error("no opening braces after paren", token[current].location);
    }
    return node;
}
