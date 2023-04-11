#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include "SyntaxAnalysis/include/Node.hpp"
#include "SemanticAnalysis/include/SymbolTable.hpp"
#include "Register.hpp"

#include <vector>
#include <string>

class CodeGen 
{
    public:
        explicit CodeGen(const symbolTable::Table& symboltable): _symboltable(symboltable) {
            reg.free_all_registers();
            last reg = -1;
        }

        std::string convert(const std::vector<std::shared_ptr<Node>> &compound);

        void traverse(const std::shared_ptr<Node> node);

        void generateAdd(const std::shared_ptr<Node>& left, const std::shared_ptr<Node>& right);

        void perform_operation(LexerTokenType op, int reg_idx1, int reg_idx2);

        std::string output() {
            return output_;
        }

    private:
        std::string output_;
        RegisterAllocator reg;
        int last_reg;
        symbolTable::Table _symboltable;
};

#endif
