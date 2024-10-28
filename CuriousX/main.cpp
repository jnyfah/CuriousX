
#include "Compiler.hpp"
#include "CompilerOutput.hpp"
#include <iostream>
#include <sstream>

std::string processFileContent(const std::string& content)
{
    CompilerOutput output;
    Compiler       compiler(content, output);
    compiler.compile();
    return output.getJson().dump();
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

    CompilerOutput output(argv[1]);
    auto           content = output.readFromFile();
    Compiler       compiler(content, output);
    compiler.compile();
    output.writeToFile(argv[2]);

    return 0;
}
#endif
