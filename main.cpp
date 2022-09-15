#include "FileHandler.hpp"
#include "LexicalAnalysis/include/Lexer.hpp"
#include <iostream>
#include <sstream>

#ifdef 	_MSC_VER
#include "SyntaxAnalysis/src/Parser.cpp"
#else
#include "SyntaxAnalysis/include/Parser.hpp"
#endif



int main(int argc, const char *argv[])
{
  try {
    FileHandler fileHandler;

    if (fileHandler.ParseArguments(argc, argv)) {

      Lexer::Lexer lex(fileHandler.getFileContents());

      std::vector<LexerToken> m_tokens;

      for (auto token = lex.nextNWToken(); token.type != LexerTokenType::Eof; token = lex.nextNWToken()) {
        m_tokens.emplace_back(token);
      }
      std::cout<<m_tokens.size()<<std::endl;
      fileHandler.LexerFile(m_tokens);
      Parser parsed(m_tokens);
      if(parsed.Parse()) {
        printTree(std::move(parsed.root), 0);
        displayInOrder(std::move(parsed.root));


      } else {
        std::cout <<"bojack \n";
      }


   
     
    }
  } catch (Error &ex) {
    std::cerr << ex.getErrorMessage() << std::endl;
  }
}

