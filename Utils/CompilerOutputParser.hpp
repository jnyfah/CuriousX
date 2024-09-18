#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include "LexerToken.hpp"

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

        for (const auto& token : j["lexer"])
        {
            outFile << std::left << std::setw(20) << token["type"].get<std::string>()
                    << std::setw(20) << std::setw(20) << token["location"].get<std::string>()
                    << formatValue(token["value"].get<std::string>()) << "\n";
        }

        outFile.close();
        std::cout << "Lexer output written to: " << outputFile << std::endl;
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
        jsonOutput["error"] = std::string("An error occurred: ") + ex.what();
    }

    void SetLexerOutput(const LexerToken& token)
    {
        jsonOutput["lexer"].push_back(serializeLexerToken(token));
    }
};
