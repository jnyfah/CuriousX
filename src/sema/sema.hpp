#pragma once
#include <cstddef>
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
        void      analyze(ProgramNode* root);

        ValueType analyzeExpr(Node* node); // produces a type
        void      analyzeStmt(Node* node); // produces nothing
        ValueType analyzeBinary(BinaryNode* node);
        ValueType inferIdentifier(Node* node);
        ValueType analyzeUnary(UnaryNode* node);
        ValueType analyzeFunCall(CallNode* node);
        void      analyzeIf(IfNode* node);
        void      analyzeWhile(WhileNode* node);
        void      analyzeReturn(ReturnNode* node);
        void      analyzePrint(PrintNode* node);
        void      analyzeFunDclr(FuncNode* node);

        //! The analysed program: function signatures, locals, frame layout inputs.
        const SymbolTable& table() const { return m_table; }

    private:
        void         analyzeFunctionBody(size_t index);
        Diagnostics& m_diag;
        SymbolTable  m_table;
    };

} // namespace cx