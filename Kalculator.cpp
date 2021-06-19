// Kalculator.cpp : Defines the entry point for the application.
//

#include "Kalculator.h"
#include <fstream>

using namespace std;

int main()
{
    Lexer lex;
    std::string input = "resource/input.txt";

    std::fstream file(input);

    if (!file.is_open()) {
        std::cerr << "BAD_TYPE_FILE" << std::endl;
    }

    std::vector<Token>m_tok;

    std::string line;
    int row = 1;
    while (std::getline(file, line)) {
        m_tok = lex.Tokenize(line, row);
        row++;
        for (auto x : m_tok) {
            std::cout << std::left << std::setw(10) << "[" + x.value + "]" << std::setw(15) << lex.toString(x.type) << std::setw(10) << x.source.toString() << std::endl;

        }
    }

}
