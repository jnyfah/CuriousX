#include "Codegen.hpp"
#include "CompilerOutputParser.hpp"
#include "Lexer.hpp"
#include "Semantic.hpp"



#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

#include <sstream>

#ifdef _MSC_VER
#include "Parser.cpp"
#else
#include "Parser.hpp"
#endif


nlohmann::json serializeLexerToken(const LexerToken &token)
{
    nlohmann::json j;
    j["type"] = toString(token.type);
    j["location"] = token.location.toString();
    j["value"] = token.value;

    return j;
}

nlohmann::json nodeToJson(const std::shared_ptr<Node> &node)
{
    if (!node) return {};

    nlohmann::json j;
    j["type"] = serializeLexerToken(node->type);
    if (node->left) { j["left"] = nodeToJson(node->left); }
    if (node->right) { j["right"] = nodeToJson(node->right); }
    return j;
}

nlohmann::json treesToJson(const std::vector<std::shared_ptr<Node>> &compound)
{
    nlohmann::json jsonArray = nlohmann::json::array();

    for (const auto &rootNode : compound) { jsonArray.push_back(nodeToJson(rootNode)); }

    return jsonArray;
}

std::string processFileContent(const std::string &content)
{
    std::ostringstream output;
    try
    {
        nlohmann::json j;
        j["lexer"] = nlohmann::json::array();
        auto lexer = new Lexer(content);
        std::vector<LexerToken> m_tokens;

        for (auto token = lexer->nextNWToken(); token.type != LexerTokenType::Eof; token = lexer->nextNWToken())
        {
            j["lexer"].push_back(serializeLexerToken(token));
            m_tokens.emplace_back(token);
        }


        auto ast = new Parser(m_tokens);
        if (ast->Parse())
        {
            j["syntax"] = treesToJson(ast->astRoot());
        } else
        {

            throw Error("Parsing failed ");
        }

        auto sem = new Semantic();
        sem->traverse(ast->astRoot());
        // Print table for debugging ;D
        j["semantic"] = sem->printSymbolTree();

        auto cg = new CodeGen(sem->getSymbolTable());
        j["codegen"] = cg->convert(ast->astRoot());


        output << j.dump();
    } catch (Error &ex)
    {
        nlohmann::json j{ { "success", false }, { "error", std::string("An error occurred: ") + ex.what() } };
        output.str("");// Clearing the stringstream
        output << j.dump();
    }
    return output.str();
}

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_BINDINGS(my_module) { emscripten::function("processFileContent", &processFileContent); }
#else
int main(int argc, const char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }
    try
    {
        std::string jsonString = processFileContent(CompilerOutputParser::readInputFile(argv[1]));
        CompilerOutputParser::parseAndSaveJson(jsonString, argv[2]);
    } catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
#endif