#include "Parser.hpp"

std::unique_ptr<Node> Parser::makeNode(std::string value, NodeType type, Node *left, Node *right, SourceLocation location) {
    auto node = std::make_unique<Node>();
    node->value = value;
    node->type = type;
    node->left = left;
    node->right = right;
    node->location = location;

    return node;
}

std::unique_ptr<Node>Parser::makeLeafNode(std::string value, NodeType type, SourceLocation location) {
    return (makeNode(value, type, nullptr, nullptr, location));
}

std::unique_ptr<Node>Parser::makeUnaryNode(std::string value, NodeType type, Node *left, SourceLocation location) {
    return (makeNode(value, type, left, nullptr, location));
}