// Kalculator.cpp : Defines the entry point for the application.
//

#include "Kalculator.hpp"
#include <iostream>
#include <iomanip>
#include <vector>

#include <fstream>



using namespace std;

int main() {
  
  FileHandler fh("input.txt");
  
 
   Lexer lex(fh.scan());
    //for (auto t = lex.nextToken(); t.type != LexerTokenType::Eof; t = lex.nextToken()) {
        //auto vv = "[" +  std::string(t.value) + "]";
        //std::cout << std::left << std::setw(6) << vv << " ->   " << t.location.toString() << ";\t " << toString(t.type) << std::endl;
    //}

    std::vector<LexerToken> m_tokens;
     for (auto token = lex.nextNWToken(); token.type != LexerTokenType::Eof; token = lex.nextNWToken()) {
                m_tokens.emplace_back(token);
            }
          

    

    for (auto x : m_tokens) {
        auto vv = "[" +  std::string(x.value) + "]";
        std::cout << std::left << std::setw(6) << vv << " ->   " << x.location.toString() << ";\t " << toString(x.type) << std::endl;
    }
    std::cout<<"Done"<<std::endl;
    return 0;
   
}