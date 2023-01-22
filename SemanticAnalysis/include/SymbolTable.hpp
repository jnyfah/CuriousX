/****
 **
 ** @copyright copyright (c) 2022
 **
 **
 ** Distributed under the Boost Software License, Version 1.0.
 ** (See accompanying file LICENSE_1_0.txt or copy at
 ** http://www.boost.org/LICENSE_1_0.txt)
 **
 **
 ** @author Jennifer Chukwu
 ** @email: <jnyfaah@gmail.com>
 **
 ** see https://github.com/jnyfah/CuriousX for most recent version including documentation.
 ** Project CuriousX...2022
 **
 */

#ifndef _SYMBOL_TABLE_HPP_
#define _SYMBOL_TABLE__HPP_

///////////////////////////////////////////////////////////////////////////
/// This file contains implementation of the helper logic data structure
/// that allows us to check if a symbol was already defined
///////////////////////////////////////////////////////////////////////////

#include <memory>
#include <optional>

#include "Error.hpp"
#include "LexicalAnalysis/include/LexerToken.hpp"


namespace symbolTable {

    class Node
    {
      public:
        LexerToken node;
        std::string inferredType;
        std::shared_ptr<Node> next;

        Node(LexerToken node) : node(node), inferredType(""), next(nullptr) {}
    };


    class Table
    {

      private:
        std::shared_ptr<Node> root;

      public:
        void insert(LexerToken node, const std::string &inferredType)
        {

            auto newNode = std::make_shared<Node>(node);
            newNode->inferredType = inferredType;

            if (root == nullptr)
            {
                root = newNode;
            } else if (node.value < root->node.value)
            {
                newNode->next = root;
                root = newNode;
            } else
            {
                auto current = root;
                while (current->next != nullptr && current->next->node.value < node.value) { current = current->next; }
                if (current->node.value == node.value) { throw Error(" Warning: Variable Already Defined! "); }
                newNode->next = current->next;
                current->next = newNode;
            }
        }

        bool search(LexerToken node)
        {
            auto current = root;
            while (current != nullptr)
            {
                if (current->node.value == node.value) { return true; }
                current = current->next;
            }
            return false;
        }

        // Get inferred type
        std::optional<std::string> getInferredType(std::string varName)
        {
            auto current = root;

            while (current != nullptr)
            {
                if (current->node.value == varName) { return current->inferredType; }
                current = current->next;
            }
            return std::nullopt;
        }

        // set inferred type
        void setInferredType(std::string varName, std::string inferredType)
        {
            auto current = root;

            while (current != nullptr)
            {
                if (current->node.value == varName)
                {
                    current->inferredType = inferredType;
                    return;
                }
                current = current->next;
            }
        }
    };
};// namespace symbolTable

#endif