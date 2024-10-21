
#include "Compiler.hpp"
#include "CompilerOutputParser.hpp"
#include <iostream>
#include <sstream>

std::string processFileContent(const std::string& content)
{
    std::ostringstream output;
    try
    {
        CompilerOutput m_output;
        Compiler com(content, m_output);
        com.compile();

        m_output.writeToFile("C:/Repos/CuriousX/CompilerUtils/output.txt");
    }
    catch (const Error& ex)
    {
        std::cout << ex.what();
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
        
        std::ifstream inputFile(argv[1]);
        std::ostringstream sstr;
        sstr << inputFile.rdbuf();
    
        std::string jsonString = processFileContent(sstr.str());
        std::cout<<jsonString;
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
#endif
