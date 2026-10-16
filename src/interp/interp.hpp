#pragma once
#include <charconv>
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

    inline Value valueOf(const Node* node)
    {
        switch (node->kind)
        {
            case NodeKind::Number:
            {
                if (node->valuetype == ValueType::Float)
                {
                    double d = 0.0;
                    std::from_chars(node->token.value.data(), node->token.value.data() + node->token.value.size(), d);
                    return d;
                }

                std::int32_t i = 0;
                std::from_chars(node->token.value.data(), node->token.value.data() + node->token.value.size(), i);
                return i;
            }
            case NodeKind::Bool:
                return node->token.value == "true";
            case NodeKind::String:
                return std::string{node->token.value};
            default:
                return std::monostate{};
        }
    }
} // namespace cx