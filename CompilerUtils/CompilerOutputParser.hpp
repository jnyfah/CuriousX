#pragma once

#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

#include "Node.hpp"
#include "SymbolTable.hpp"

using json = nlohmann::json;

class CompilerOutputParser
{
  private:
    CompilerOutputParser()
    {
        jsonOutput["Lexer"] = nlohmann::json::array();
        jsonOutput["success"] = true;
    }

    static std::string formatValue(const std::string& value)
    {
        if (value.empty())
            return "<empty>";
        if (value == "\n")
            return "\\n";
        if (value == "\t")
            return "\\t";
        return "[" + value + "]";
    }

    nlohmann::json jsonOutput;

  public:
    static CompilerOutputParser& getInstance()
    {
        static CompilerOutputParser instance;
        return instance;
    }

    CompilerOutputParser(const CompilerOutputParser&) = delete;
    CompilerOutputParser& operator=(const CompilerOutputParser) = delete;

    static std::string readInputFile(const std::string& filePath)
    {
        std::ifstream inputFile(filePath);
        if (!inputFile.is_open())
        {
            throw std::runtime_error("Unable to open input file: " + filePath);
        }
        std::ostringstream sstr;
        sstr << inputFile.rdbuf();
        return sstr.str();
    }

    std::string getJson() const { return jsonOutput.dump(); }

    static void formatTokens(const std::string& jsonString, const std::string& outputFile)
    {
        nlohmann::json j = nlohmann::json::parse(jsonString);
        std::ofstream outFile(outputFile);
        if (!outFile.is_open())
        {
            throw std::runtime_error("Unable to open output file: " + outputFile);
        }

        outFile << "==== Lexer Output ====\n\n";
        outFile << std::left << std::setw(20) << "Token" << std::setw(20) << "Position"
                << "Value\n";
        outFile << std::string(70, '-') << "\n";

        for (const auto& token : j["Lexer"])
        {
            outFile << std::left << std::setw(20) << token["type"].get<std::string>()
                    << std::setw(20) << std::setw(20) << token["location"].get<std::string>()
                    << formatValue(token["value"].get<std::string>()) << "\n";
        }

        outFile << "==== AST Output ====\n\n";
        drawASTNode(j["AST"], outFile, 0);

        outFile << "==== Symbol Output ====\n\n";
        drawTable(j["SymbolTable"], outFile);

        outFile.close();
        std::cout << "output written to: " << outputFile << std::endl;
    }

    static void drawASTNode(const nlohmann::json& node, std::ofstream& outFile, int depth)
    {
        if (node.is_null())
            return;

        std::string indent(depth * 2, ' ');
        outFile << indent << "└─ ";

        // Print token information
        outFile << node["token"]["value"].get<std::string>() << "\n";

        // Handle different node types
        if (node.contains("children"))
        {
            for (const auto& child : node["children"])
            {
                drawASTNode(child, outFile, depth + 1);
            }
        }
        else if (node.contains("left") && node.contains("right"))
        {
            drawASTNode(node["left"], outFile, depth + 1);
            drawASTNode(node["right"], outFile, depth + 1);
        }
        else if (node.contains("condition") && node.contains("ifNode") && node.contains("elseNode"))
        {
            drawASTNode(node["condition"], outFile, depth + 2);
            drawASTNode(node["ifNode"], outFile, depth + 2);
            drawASTNode(node["elseNode"], outFile, depth + 2);
        }
    }

    nlohmann::json serializeLexerToken(const LexerToken& token)
    {
        nlohmann::json j;
        j["type"] = toString(token.type);
        j["location"] = token.location.toString();
        j["value"] = token.value;

        return j;
    }

    void setErrorOutput(const Error& ex)
    {
        jsonOutput["success"] = false;
        jsonOutput["error"] = ex.what();
    }

    void SetLexerOutput(const LexerToken& token)
    {
        jsonOutput["Lexer"].push_back(serializeLexerToken(token));
    }

    void setASTOutput(const std::unique_ptr<TreeNode>& root,
                      const std::vector<std::unordered_map<std::string, SymbolInfo>>& table)
    {
        jsonOutput["AST"] = nodeToJson(root);
        jsonOutput["SymbolTable"] = tableToJson(table);
    }

    nlohmann::json
    tableToJson(const std::vector<std::unordered_map<std::string, SymbolInfo>>& table)
    {
        nlohmann::json jArray = nlohmann::json::array();
        nlohmann::json j;

        for (const auto& node : table)
        {
            for (auto it : node)
            {
                j["value"] = it.first;
                j["type"] = getInferredTypeDescription(it.second.type);
                jArray.push_back(j);
            }
        }
        return jArray;
    }

    static std::string getNodeTypeName(NodeType type)
    {
        switch (type)
        {
        case NodeType::BinaryOperation:
            return "BinaryOperation";
        case NodeType::ConditionalOperation:
            return "ConditionalOperation";
        case NodeType::BlockOperation:
            return "BlockOperation";
        default:
            return "Unknown";
        }
    }

    nlohmann::json nodeToJson(const std::unique_ptr<TreeNode>& node)
    {
        if (!node)
            return nullptr;

        nlohmann::json j;
        j["type"] = getNodeTypeName(node->getType());
        j["token"] = {{"type", toString(node->token.type)},
                      {"value", node->token.value},
                      {"location", node->token.location.toString()}};

        j["children"] = nlohmann::json::array();
        for (const auto& child : node->children)
        {
            j["children"].push_back(nodeToJson(child));
        }

        return j;
    }

    nlohmann::json nodeToJson(const std::unique_ptr<ASTNode>& node)
    {
        if (!node)
            return nullptr;

        nlohmann::json j;
        j["type"] = getNodeTypeName(node->getType());
        j["token"] = {{"type", toString(node->token.type)},
                      {"value", node->token.value},
                      {"location", node->token.location.toString()}};

        switch (node->getType())
        {
        case NodeType::BinaryOperation:
        {
            const auto& binaryNode = static_cast<const BinaryNode&>(*node);
            j["left"] = nodeToJson(binaryNode.left);
            j["right"] = nodeToJson(binaryNode.right);
            break;
        }
        case NodeType::ConditionalOperation:
        {
            const auto& condNode = static_cast<const ConditionalNode&>(*node);
            j["condition"] = nodeToJson(condNode.condition);
            j["ifNode"] = nodeToJson(condNode.ifNode);
            j["elseNode"] = nodeToJson(condNode.elseNode);
            break;
        }
        case NodeType::BlockOperation:
            const auto& printNode = static_cast<const TreeNode&>(*node);
            j["children"] = nlohmann::json::array();
            for (const auto& child : printNode.children)
            {
                j["children"].push_back(nodeToJson(child));
            }
            break;
        }

        return j;
    }

    std::string_view getInferredTypeDescription(const InferredType& t)
    {
        switch (t)
        {
        case InferredType::BOOL:
            return "Boolean";
        case InferredType::FLOAT:
            return "Float";
        case InferredType::INTEGER:
            return "Integer";
        case InferredType::STRING:
            return "String";
        default:
            return "An unknown error occurred";
        }
    }

    static void drawTable(const nlohmann::json& node, std::ofstream& outFile)
    {
        if (!node.is_array() || node.empty())
        {
            outFile << "No data to display\n";
            return;
        }

        // Determine the maximum width for each column
        size_t typeWidth = 4;  // Minimum width for "Type" header
        size_t valueWidth = 5; // Minimum width for "Value" header
        for (const auto& item : node)
        {
            typeWidth = std::max(typeWidth, item["type"].get<std::string>().length());
            valueWidth = std::max(valueWidth, item["value"].get<std::string>().length());
        }

        // Draw the header
        outFile << std::setfill('-') << std::setw(typeWidth + valueWidth + 7) << "-" << "\n";
        outFile << std::setfill(' ');
        outFile << "| " << std::left << std::setw(typeWidth) << "Type"
                << " | " << std::setw(valueWidth) << "Value" << " |\n";
        outFile << std::setfill('-') << std::setw(typeWidth + valueWidth + 7) << "-" << "\n";
        outFile << std::setfill(' ');

        // Draw the rows
        for (const auto& item : node)
        {
            outFile << "| " << std::left << std::setw(typeWidth) << item["type"].get<std::string>()
                    << " | " << std::setw(valueWidth) << item["value"].get<std::string>() << " |\n";
        }

        // Draw the bottom border
        outFile << std::setfill('-') << std::setw(typeWidth + valueWidth + 7) << "-" << "\n";
    }
};
