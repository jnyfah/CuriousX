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
#define _SYMBOL_TABLE_HPP_

///////////////////////////////////////////////////////////////////////////
/// This file contains implementation of the helper logic data structure
/// that allows us to check if a symbol was already defined
///////////////////////////////////////////////////////////////////////////

#include <memory>
#include <optional>
#include <variant>

#include "Error.hpp"
#include "LexicalAnalysis/include/LexerToken.hpp"
#include "SyntaxAnalysis/include/Node.hpp"


namespace symbolTable {

    class Node
    {
        public:
            LexerToken node;
            std::variant<InferredType, std::monostate> inferredType;
            std::shared_ptr<Node> next;

            explicit Node(LexerToken const &node) : node(node), inferredType(std::monostate{}), next(nullptr) {}
        };



    class Table
    {

      private:
        std::shared_ptr<Node> root;

      public:
        void insert(LexerToken node, const InferredType &inferredType)
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
        std::optional<InferredType> getInferredType(const std::string &varName)
        {
            auto current = root;

            while (current != nullptr)
            {
                if (current->node.value == varName)
                {
                    if (std::holds_alternative<InferredType>(current->inferredType))
                    {
                        return std::get<InferredType>(current->inferredType);
                    } else
                    {
                        return std::nullopt;
                    }
                }
                current = current->next;
            }
            return std::nullopt;
        }

        // set inferred type
        void setInferredType(const std::string &varName, InferredType &inferredType)
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


        // Print symbol table helper function to get types
        std::string to_string(const InferredType& inferredType) {
            if (inferredType == InferredType::INTEGER) {
                return "INT";
            } else if (inferredType == InferredType::FLOAT) {
                return "FLOAT";
            }
            // add cases for other possible values of InferredType
            return "UNKNOWN";
        }

        // prints symbol table
        void printTable(std::shared_ptr<Node> head) {
            std::shared_ptr<Node> current = head;
            while (current != nullptr) {
                std::cout << current->node.value << " " << to_string(std::get<InferredType>(current->inferredType));
                current = current->next;
                std::cout << std::endl;
            }
        }

        std::shared_ptr<Node> getRootNode() { return root; }
    };
};// namespace symbolTable

#endif