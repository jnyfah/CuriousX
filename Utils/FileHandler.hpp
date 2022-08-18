/****
 **
 ** @copyright copyright (c) 2022
 **
 **
 ** Distributed under the Boost Software License, Version 1.0.
 ** (See accompanying file LICENSE_1_0.txt or copy at
 ** http://www.boost.org/LICENSE_1_0.txt)
 **
 **
 ** @author Jennifer Chukwu
 ** @email: <jnyfaah@gmail.com>
 **
 ** see https://github.com/jnyfah/CuriousX for most recent version including documentation.
 ** Project CuriousX...2022 
 **
 */


#ifndef __FILE_HANDLER_HPP_
#define __FILE_HANDLER_HPP_


///////////////////////////////////////////////////////////////////////////
/// This file contains implementation of the helper logic for argument parser
/// and file handling used internally.
///////////////////////////////////////////////////////////////////////////


#include <filesystem>
#include <fstream>
#include <string_view>
#include <iostream>
#include <vector>
#include <sstream>


#include "../LexicalAnalysis/include/LexerToken.hpp"




namespace fs = std::filesystem;

class FileHandler {

    public:
        FileHandler() {}

        bool ParseArguments(int argc, const char* argv[]) {
            int start = 0;

            if(argc < 2) {
                std::cerr<< "no input file specified" <<std::endl; // Todo exception
                return false;
                
            }

            std::string_view option = argv[start];   
            filename = argv[++start];

            if(!fs::is_regular_file(fs::path(filename)) || fs::path(filename).extension() != ".txt") {
                std::cerr<< "invalid input file path" << std::endl;
                return false;
            }

            inputFile.open(filename);

            if (!inputFile.is_open()) {
                std::cerr << " cannot open audio file \n";  // Todo exception
                return false;
            }
            

            return true;
        }

        std::string getFileContents(){
            std::ostringstream sstr;
            sstr << inputFile.rdbuf();
            return sstr.str();
        }

        bool LexerFile(std::vector<LexerToken> m_tokens) {
          std::fstream outputFile;

          outputFile.open("Lexical-analysis.txt", std::ios_base::out);
          if (!outputFile.is_open()) {
            std::cout << "failed to create lexical analysis file \n"; // Todo exception
            return false;
          } 

           for (auto x : m_tokens) {
                auto vv = "[" +  std::string(x.value) + "]";
             outputFile << std::left << std::setw(6) << vv << " ->   " << x.location.toString() << ";\t "
                      << toString(x.type) << std::endl;
            }
            return true;
        }

        std::string getFilename(){
            return filename;
        }


    private:
        std::string filename;
        std::fstream inputFile;
        

};

#endif
