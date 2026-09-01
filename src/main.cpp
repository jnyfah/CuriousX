#include <iostream>
#include "parser/parser.hpp"

using namespace cx;
int main()
{
    Diagnostics  d;
    Arena        arena(40000);
    Lexer        lx("func add(a, b) { return a + b; }\nx = add(1, 2);\nif (x) { print x; }", d);
    Parser       p(lx, d, arena);

    ProgramNode* root = p.Parse();
    std::cout << "statements: " << root->statements.size() << "  diagnostics: " << d.all().size() << "\n";
    for (const auto& x : d.all())
    {
        std::cout << x.loc.toString() << " " << x.message << "\n";
    }
}
