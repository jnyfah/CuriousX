#include "FileHandler.hpp"
#include "LexicalAnalysis/include/Lexer.hpp"
#include "SemanticAnalysis/include/Semantic.hpp"
#include <iostream>
#include <sstream>

#ifdef _MSC_VER
#include "SyntaxAnalysis/src/Parser.cpp"
#else
#include "SyntaxAnalysis/include/Parser.hpp"
#endif


int main(int argc, const char *argv[])
{
    try
    {
        auto fileHandler = new FileHandler;

        if (fileHandler->ParseArguments(argc, argv))
        {
            auto lexer = new Lexer(fileHandler->getFileContents());

            std::vector<LexerToken> m_tokens;

            for (auto token = lexer->nextNWToken(); token.type != LexerTokenType::Eof; token = lexer->nextNWToken())
            { m_tokens.emplace_back(token); }

            fileHandler->LexerFile(m_tokens);
            auto ast = new Parser(m_tokens);

            if (ast->Parse())
            {
                fileHandler->SyntaxFile(ast->astRoot());
            } else {

                throw Error("Parsing failed ");
            }
            auto sem = new Semantic();
            sem->traverse(ast->astRoot());
        }
    } catch (Error &ex)
    {
        std::cerr << ex.getErrorMessage() << std::endl;
    }
}