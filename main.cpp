#include "FileHandler.hpp"
#include "LexicalAnalysis/include/Lexer.hpp"
#include <iostream>
#include <vector>





int main(int argc, const char* argv[]) { 
    
    FileHandler fileHandler;

    

    if(fileHandler.ParseArguments(argc, argv)) {
       
    Lexer lex(fileHandler.getFileContents());

    std::vector<LexerToken> m_tokens;
     for (auto token = lex.nextNWToken(); token.type != LexerTokenType::Eof; token = lex.nextNWToken()) {
            m_tokens.emplace_back(token);
    }

    for (auto x : m_tokens) {
        auto vv = "[" +  std::string(x.value) + "]";
        std::cout << std::left << std::setw(6) << vv << " ->   " << x.location.toString() << ";\t " << toString(x.type) << std::endl;
    }
    }
 }
