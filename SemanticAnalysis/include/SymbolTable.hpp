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
#include <set>
#include <sstream>
#include <variant>

#include "Error.hpp"
#include "json.hpp"
#include "LexicalAnalysis/include/LexerToken.hpp"
#include "SyntaxAnalysis/include/Node.hpp"


namespace symbolTable {

    struct Node
    {
        LexerToken node;
        std::variant<InferredType, std::monostate> inferredType;

        explicit Node(LexerToken const &node) : node(node), inferredType(std::monostate{}) {}

        bool operator<(const Node &other) const { return node.value < other.node.value; }
    };

    class Table
    {

      private:
        std::set<Node> nodes;

      public:
        void insert(const LexerToken &node, const InferredType &inferredType)
        {

            auto newNode = Node(node);
            newNode.inferredType = inferredType;

            if (!nodes.insert(newNode).second) { throw Error(" Warning: Variable Already Defined! "); }
        }

        bool search(const LexerToken &node)
        {
            auto searchNode = Node(node);
            return nodes.find(searchNode) != nodes.end();
        }

        // Get inferred type
        std::optional<InferredType> getInferredType(const std::string &varName)
        {
            for (const auto &node : nodes)
            {
                if (node.node.value == varName)
                {
                    if (std::holds_alternative<InferredType>(node.inferredType))
                    {
                        return std::get<InferredType>(node.inferredType);
                    } else
                    {
                        return std::nullopt;
                    }
                }
            }
            return std::nullopt;
        }


        // Print symbol table helper function to get types
        std::string to_string(const InferredType &inferredType)
        {
            if (inferredType == InferredType::INTEGER)
            {
                return "INT";
            } else if (inferredType == InferredType::FLOAT)
            {
                return "FLOAT";
            }
            // add cases for other possible values of InferredType
            return "UNKNOWN";
        }

        nlohmann::json tableToJson()
        {
            nlohmann::json jArray = nlohmann::json::array();

            for (const auto &node : nodes)
            {
                nlohmann::json jNode;
                jNode["value"] = node.node.value;
                jNode["type"] = to_string(std::get<InferredType>(node.inferredType));

                jArray.push_back(jNode);
            }

            return jArray;
        }


        std::set<Node> getSymbolTable() { return nodes; }
    };
};// namespace symbolTable

#endif
