#include "FileHandler.hpp"
#include "LexicalAnalysis/include/Lexer.hpp"
#include <iostream>





int main(int argc, const char* argv[]) { 
    FileHandler fileHandler;
    if(fileHandler.ParseArguments(argc, argv)) {
        std::cout<< "yes" <<std::endl;
    }else {
        std::cout<< "no" <<std::endl;
    }

    std::cout<< argc <<std::endl;
    //std::cout << "Curious X \n"; 
 }