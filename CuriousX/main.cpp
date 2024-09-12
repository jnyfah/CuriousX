#include "CompilerOutputParser.hpp"
#include "Lexer.hpp"
#include <sstream>

nlohmann::json serializeLexerToken(const LexerToken& token)
{
    nlohmann::json j;
    j["type"] = toString(token.type);
    j["location"] = token.location.toString();
    j["value"] = token.value;

    return j;
}

std::string processFileContent(const std::string& content)
{
    std::ostringstream output;
    try
    {
        nlohmann::json j;
        j["lexer"] = nlohmann::json::array();
        Lexer lexer(content);
        for (auto token = lexer.nextNWToken(); token.type != LexerTokenType::Eof;
             token = lexer.nextNWToken())
        {
            j["lexer"].push_back(serializeLexerToken(token));
        }
        output << j.dump();
    }
    catch (const Error& ex)
    {
        nlohmann::json j{{"success", false},
                         {"error", std::string("An error occurred: ") + ex.what()}};
        output.str("");
        output << j.dump();
    }
    return output.str();
}

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
EMSCRIPTEN_BINDINGS(my_module)
{
    emscripten::function("processFileContent", &processFileContent);
}
#else
int main(int argc, const char* argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
    }
    try
    {
        std::string jsonString = processFileContent(CompilerOutputParser::readInputFile(argv[1]));
        CompilerOutputParser::formatTokens(jsonString, argv[2]);
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
#endif
