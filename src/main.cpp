#include <iostream>
#include "lower/lower.hpp"
#include "parser/parser.hpp"
#include "riscv/asm.h"
#include "sema/sema.hpp"

using namespace cx;
int main()
{
    Diagnostics d;
    Arena       arena(40000);

    // comparisons: fused when a JumpIFF follows (if/while), stored as a bool otherwise (assignment)
    Lexer       lx("x = 0;\n"
                 //  "while (x < 10) { x = x + 1; }\n"
                   "if (x > 5) { x = 1; }\n",
                 //  "if (x == 1) { x = 2; } else { x = 3; }\n"
                  // "if (x != 2) { x = 4; }\n"
                  // "if (x <= 2) { x = 5; }\n"
                  // "if (x >= 5) { x = 6; }\n"
                  // "a = x < 3;\n"
                  // "b = x == 6;\n",
                   d);
    Parser      p(lx, d, arena);

    ProgramNode* root = p.Parse();
    Sema         sema(d);
    sema.analyze(root);

    if (d.hasErrors())
    {
        for (const auto& x : d.all())
        {
            std::cout << x.loc.toString() << " " << x.message << "\n";
        }
        return 1;
    }

    Lower lower(arena, sema.table());
    auto  rows = lower.MirLower(root);

    Asm   riscv(rows);
    riscv.TargetLower(lower.Layouts(), lower.MainIndex());
}
