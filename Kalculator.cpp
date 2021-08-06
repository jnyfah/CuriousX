// Kalculator.cpp : Defines the entry point for the application.
//

#include "Kalculator.hpp"
#include <iostream>
#include <iomanip>

#include <fstream>



using namespace std;

int main() {
  
  FileHandler fh("input.txt");
  
 
   Lexer lex(fh.scan());
    for (auto t = lex.nextToken(); t.type != LexerTokenType::Eof; t = lex.nextToken()) {
        auto vv = "[" +  std::string(t.value) + "]";//
        std::cout << std::left << std::setw(6) << vv << " ->   " << t.location.toString() << ";\t " << toString(t.type) << std::endl;
    }

    return 0;
   
}
