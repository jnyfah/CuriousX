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


namespace fs = std::filesystem;

class FileHandler {

    public:
        FileHandler() {}

        bool ParseArguments(int argc, const char* argv[]) {
            int start = 0;

            if(argc < 2) {
                std::cerr<< "greater??";
                return false;
                
            }

            std::string_view option = argv[start];   
            filename = argv[++start];

            if(!fs::is_regular_file(fs::path(filename)) || fs::path(filename).extension() != ".txt") {
                std::cerr<< "greater";
                return false;
            }

            inputFile.open(filename);

            if (!inputFile.is_open()) {
                std::cerr << " cannot open audio file \n";
                return false;
            }
            

            return true;
        }


    private:
        std::string filename;
        std::fstream inputFile;
        

};

#endif

// TO do: validate input file
// Read input file and get components 