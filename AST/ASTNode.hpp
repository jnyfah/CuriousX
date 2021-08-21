#ifndef AST_NODE
#define AST_NODE

#include <cassert>
#include <memory>
#include <string>
#include <vector>

#include "../Lexer/LexerToken.hpp"
#include "../utils/Operations.hpp"


class ASTNode {


    public:

        explicit ASTNode(Node nodeType) : nodeType(nodeType) { }
        virtual ~ASTNode() = default;

        ASTNode(const ASTNode&) = default;
         ASTNode(ASTNode&&) = default;
        ASTNode& operator=(const ASTNode&) = default;
        ASTNode& operator=(ASTNode&&) = default;

        const SourceLocation getLocation() const { return nodeType.location; }

        const NodeKind getNodetype() const {return nodeType.nodetype; }

        const std::string_view getName() const { return nodeType.name; }

        const std::vector<std::unique_ptr<Node>> & GetChildrenNodes() const { return nodeType.children; }

        void AddChildNode(std::unique_ptr<Node>&& node) { 
                nodeType.children.push_back({std::move(node)});
        }

        template <class T> 
        T* GetChildNodeAs(size_t childIdx) const {

            assert(childIdx < nodeType.children.size()); 
            return static_cast<T*>(nodeType.children[childIdx].get()); 
        }

    private:
        Node nodeType;

};

#endif