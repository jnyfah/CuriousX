#pragma once
#include <string>
#include <vector>
#include "parser/node.hpp"
#include "sema/symboltable.hpp"
#include "value.hpp"

namespace cx
{
    struct ReturnSignal
    {
        Value value;
    };

    struct Frame
    {
        std::vector<Value> slots;
        explicit Frame(std::size_t size) : slots(size) {}
    };

    class Interpreter
    {
    public:
        Interpreter(const SymbolTable& sym);
        void               run(const ProgramNode& root);
        void               eval(const Node* node);
        const std::string& output() const;

        Value              evalExpr(const Node* node);
        Value              evalBinary(const BinaryNode* n);
        Value              evalCall(const CallNode* node);
        Value              evalUnary(const UnaryNode* node);

        // statements
        void               evalIf(const IfNode* node);
        void               evalWhile(const WhileNode* node);
        void               evalPrint(const PrintNode* node);

    private:
        std::string        m_output;
        std::vector<Frame> m_frames;
        const SymbolTable& m_sym;
    };
} // namespace cx