#include "FileHandler.hpp"
#include "LexicalAnalysis/include/Lexer.hpp"
#include <iostream>
#include <sstream>

#ifdef _MSC_VER
#include "SyntaxAnalysis/src/Parser.cpp"
#else
#include "SyntaxAnalysis/include/Parser.hpp"
#endif


int main(int argc, const char *argv[])
{
  try {
    auto fileHandler = new FileHandler;

    if (fileHandler->ParseArguments(argc, argv)) {
      auto lexer = new Lexer(fileHandler->getFileContents());

      std::vector<LexerToken> m_tokens;

      for (auto token = lexer->nextNWToken(); token.type != LexerTokenType::Eof; token = lexer->nextNWToken()) {
        m_tokens.emplace_back(token);
      }
      fileHandler->LexerFile(m_tokens);
      Parser parsed(m_tokens);
      if (parsed.Parse()) {
        fileHandler->SyntaxFile(parsed.root);
      } else {
        throw Error("Parsing failed");
      }
    }
  } catch (Error &ex) {
    std::cerr << ex.getErrorMessage() << std::endl;
  }
}