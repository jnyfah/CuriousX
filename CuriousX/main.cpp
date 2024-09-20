
#include "Parser.hpp"
#include <sstream>
#include <iostream>



std::string processFileContent(const std::string& content)
{
    std::ostringstream output;
    try
    {
        Parser parse(content);
        parse.ParseTokens();
        
        output << CompilerOutputParser::getInstance().getJson();
    }
    catch (const Error& ex)
    {
        CompilerOutputParser::getInstance().setErrorOutput(ex);
        output << CompilerOutputParser::getInstance().getJson();
        std::cout<<ex.what();
    }
    return output.str();
}

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
EMSCRIPTEN_BINDINGS(my_module)
{
    emscripten::function("processFileContent", &processFileContent);
}
#else
int main(int argc, const char* argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
    }
    try
    {
        std::string jsonString = processFileContent(CompilerOutputParser::getInstance().readInputFile(argv[1]));
        CompilerOutputParser::getInstance().formatTokens(jsonString, argv[2]);
        std::cout<<jsonString;
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
#endif
