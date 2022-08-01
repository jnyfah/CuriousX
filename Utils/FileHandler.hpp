#include <iomanip>
#include <iostream>

class FileHandler {
    public:

        bool ParseArguments(int argc, const char* argv[]) {
            int start = 1;

            if (argc < 2 || argc > 3) {
                std::cerr << "Provide valid arguments \n";
                return false;
            }
            std::string_view option = argv[start];
            inputFile = argv[start+1];

            if(fs::is_regular_file(fs::path(inputFile))) {
            }



            return true;
        }

        std::string load () {
            file = fopen(inputFile, "r");
            checkFile(file, inputFile);

            if (!file) {
                std::cerr << "Error: Cannot open file '" << inputFile << "'.\n";
                exit(1);
            }

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

    ~FileHandler() {
        fclose(file);
    }

    private:
        FILE *file;
        std::string inputFile;
};