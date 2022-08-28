#include "FileHandler.hpp"
#include "LexicalAnalysis/include/Lexer.hpp"
#include <iostream>
#include <sstream>





int main(int argc, const char* argv[]) { 
    try{
        FileHandler fileHandler;

        if(fileHandler.ParseArguments(argc, argv)) {
       
            Lexer lex(fileHandler.getFileContents());

            std::vector<LexerToken> m_tokens;
            for (auto token = lex.nextNWToken(); token.type != LexerTokenType::Eof; token = lex.nextNWToken()) {
                m_tokens.emplace_back(token);
            }
            if (fileHandler.LexerFile(m_tokens)) { 
                std::cout << "yes";
            }
    }
    }catch(Error& ex){
        std::cerr<< ex.getErrorMessage()<<std::endl;
    }
    

 }


