#pragma once

#include "Lexer.hpp"
#include <memory>
#include <vector>

enum class NodeType
{
    BinaryOperation,
    ConditionalOperation,
    BlockOperation,
};

class ASTNode
{
  public:
    ASTNode(const LexerToken& token) : token(token) {}
    virtual ~ASTNode()               = default;
    virtual NodeType getType() const = 0;
    LexerToken       token;
};

class BinaryNode : public ASTNode
{
  public:
    BinaryNode(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right, const LexerToken& token)
        : ASTNode(token), left(std::move(left)), right(std::move(right))
    {
    }
    NodeType                 getType() const override { return NodeType::BinaryOperation; }
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
};

class TreeNode : public ASTNode
{
  public:
    TreeNode(std::vector<std::unique_ptr<ASTNode>> children, const LexerToken& token)
        : ASTNode(token), children(std::move(children))
    {
    }
    NodeType                              getType() const override { return NodeType::BlockOperation; }
    std::vector<std::unique_ptr<ASTNode>> children;
};

class ConditionalNode : public ASTNode
{
  public:
    ConditionalNode(std::unique_ptr<ASTNode>  condition,
                    std::unique_ptr<TreeNode> ifNode,
                    std::unique_ptr<TreeNode> elseNode,
                    const LexerToken&         token)
        : ASTNode(token), condition(std::move(condition)), ifNode(std::move(ifNode)), elseNode(std::move(elseNode))
    {
    }
    NodeType                  getType() const override { return NodeType::ConditionalOperation; }
    std::unique_ptr<ASTNode>  condition;
    std::unique_ptr<TreeNode> ifNode;
    std::unique_ptr<TreeNode> elseNode;
};

class ASTNodeFactory
{
  public:
    static std::unique_ptr<BinaryNode>
    createBinaryNode(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right, const LexerToken& token)
    {
        return std::make_unique<BinaryNode>(std::move(left), std::move(right), token);
    }
    static std::unique_ptr<ASTNode> createConditionalNode(std::unique_ptr<ASTNode>  condition,
                                                          std::unique_ptr<TreeNode> ifNode,
                                                          std::unique_ptr<TreeNode> elseNode,
                                                          const LexerToken&         token)
    {
        return std::make_unique<ConditionalNode>(std::move(condition), std::move(ifNode), std::move(elseNode), token);
    }
    static std::unique_ptr<TreeNode> createTreeNode(std::vector<std::unique_ptr<ASTNode>> children,
                                                    const LexerToken&                     token)
    {
        return std::make_unique<TreeNode>(std::move(children), token);
    }
};