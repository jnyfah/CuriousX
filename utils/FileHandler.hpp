#ifndef FILE_HANDLER_HPP
#define FILE_HANDLER_HPP

#include <string>
#include <iostream>

class FileHandler {

public:
     
    FileHandler(const char* inputFile) : inputFile(inputFile) {
      file = fopen(inputFile, "r");
      checkFile(file, inputFile);
    }


    void checkFile(FILE *file, std::string inputFile) {
        if (!file) {
            std::cerr << "Error: Cannot open file '" << inputFile << "'.\n";
            exit(1);
        }
    }

    std::string scan () {
        fseek(file, 0, SEEK_END);
        size_t fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);
        std::string fileContents(fileSize, ' ');
        fread(fileContents.data(), 1, fileSize, file);

        if(fileContents.empty()) {
           std::cerr << inputFile << " is empty" <<"'.\n";
           exit(1); 
        }

        return fileContents;
    }

    ~FileHandler() {}


private:
    FILE *file;
    const char* inputFile;

};

        


#endif