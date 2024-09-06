#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <stdexcept>
#include <string>


using json = nlohmann::json;

class CompilerOutputParser
{
  private:
    static void parseSection(const std::string &section, const json &content, std::ofstream &outputFile)
    {
        outputFile << "==== " << section << " ====\n\n";

        if (section == "lexer")
        {
            parseLexer(content, outputFile);
        } else if (section == "codegen")
        {
            parseCodegen(content, outputFile);
        } else if (section == "semantic")
        {
            parseSemantic(content, outputFile);
        } else if (content.is_array())
        {
            parseArray(content, outputFile);
        } else
        {
            outputFile << content.dump(4) << "\n";
        }

        outputFile << "\n\n";
    }

    static void parseLexer(const json &content, std::ofstream &outputFile)
    {
        for (const auto &item : content)
        {
            outputFile << "[" << std::left << item["value"].get<std::string>() << "] \t -> " << std::setw(20)
                       << std::left << item["location"].get<std::string>() << " " << item["type"].get<std::string>()
                       << "\n";
        }
    }

    static void parseCodegen(const json &content, std::ofstream &outputFile)
    {
        for (const auto &instruction : content)
        {
            for (const auto &item : instruction)
            {
                if (item.is_null()) continue;
                outputFile << item["command"].get<std::string>() << " ";
                if (item.contains("register")) outputFile << item["register"].get<std::string>() << " ";
                if (item.contains("value")) outputFile << item["value"].get<std::string>();
                outputFile << "\n";
            }
        }
    }

    static void parseSemantic(const json &content, std::ofstream &outputFile)
    {
        for (const auto &item : content)
        {
            outputFile << item["type"].get<std::string>() << " -> " << item["value"].get<std::string>() << "\n";
        }
    }

    static void parseArray(const json &content, std::ofstream &outputFile)
    {
        for (const auto &item : content) { outputFile << item.dump(4) << "\n"; }
    }

  public:
    static std::string readInputFile(const std::string &filePath)
    {
        std::ifstream inputFile(filePath);
        if (!inputFile.is_open()) { throw std::runtime_error("Unable to open input file: " + filePath); }
        std::ostringstream sstr;
        sstr << inputFile.rdbuf();
        return sstr.str();
    }

    static void parseAndSaveJson(const std::string &jsonString, const std::string &outputFilePath)
    {
        try
        {
            json compilerOutput = json::parse(jsonString);
            std::ofstream outputFile(outputFilePath);
            if (!outputFile.is_open()) { throw std::runtime_error("Unable to open output file: " + outputFilePath); }

            for (const auto &[section, content] : compilerOutput.items())
            {
                parseSection(section, content, outputFile);
            }

            outputFile.close();
            std::cout << "Parsing complete. Output written to " << outputFilePath << std::endl;
        } catch (const json::parse_error &e)
        {
            throw std::runtime_error("JSON parsing error: " + std::string(e.what()));
        } catch (const std::exception &e)
        {
            throw std::runtime_error("Error parsing and saving JSON: " + std::string(e.what()));
        }
    }
};
