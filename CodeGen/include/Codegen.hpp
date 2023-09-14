#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include "SyntaxAnalysis/include/Node.hpp"
#include "SemanticAnalysis/include/SymbolTable.hpp"
#include "Register.hpp"


class CodeGen 
{
    public:
        explicit CodeGen(const symbolTable::Table& symboltable): _symboltable(symboltable) {
            reg.free_all_registers();
            last_reg = -1;
        }

        nlohmann::json convert(const std::vector<std::shared_ptr<Node>> &compound);

        nlohmann::json traverse(const std::shared_ptr<Node> node);

        void generateAdd(const std::shared_ptr<Node>& left, const std::shared_ptr<Node>& right);

        void perform_operation(LexerTokenType op, int reg_idx1, int reg_idx2);

    private:
        RegisterAllocator reg;
        int last_reg;
        symbolTable::Table _symboltable;
        std::unordered_map<std::string, int> _staked_var;
        int sp = -4;
};

#endif
