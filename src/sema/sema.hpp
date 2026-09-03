#pragma once
#include "helpers/diagnostics.hpp"
#include "helpers/type.hpp"
#include "parser/node.hpp"
#include "parser/parser.hpp"
#include "symboltable.hpp"

namespace cx
{

    class Sema
    {
    public:
        Sema(Diagnostics& d);
        void analyze(ProgramNode* root);

        ValueType analyzeExpr(Node* node); // produces a type
        void analyzeStmt(Node* node); // produces nothing
        ValueType analyzeBinary(BinaryNode* node);
        ValueType inferIdentifier(Node* node);
        ValueType analyzeUnary(UnaryNode* node);

    private:
        Diagnostics& m_diag;
        SymbolTable  m_table;
    };

} // namespace cx