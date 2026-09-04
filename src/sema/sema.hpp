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
        explicit Sema(Diagnostics& d);
        void               analyze(ProgramNode* root);

        const SymbolTable& table() const
        {
            return m_table;
        }

    private:
        ValueType    analyzeExpr(Node* node);
        ValueType    analyzeBinary(BinaryNode* node);
        ValueType    inferIdentifier(Node* node);
        ValueType    analyzeUnary(UnaryNode* node);
        ValueType    analyzeFunCall(CallNode* node);

        void         analyzeStmt(Node* node);
        void         analyzeIf(IfNode* node);
        void         analyzeWhile(WhileNode* node);
        void         analyzeReturn(ReturnNode* node);
        void         analyzePrint(PrintNode* node);
        void         analyzeFunDclr(FuncNode* node);

        void         analyzeFunctionBody(size_t index);
        Diagnostics& m_diag;
        SymbolTable  m_table;
    };

} // namespace cx