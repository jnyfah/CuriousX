// Compiler.hpp
#pragma once
#include "Generation/Codegen.hpp"
#include "Parser/Parser.hpp"
#include "Semantic/Semantic.hpp"
#include <string_view>

class Compiler
{
  public:
    Compiler(std::string_view source, CompilerOutput& output);

    bool compile();
    void collectOutputs();
    void processNode(std::unique_ptr<ASTNode>& node);

  private:
    Parser                    m_parser;
    Semantic                  m_semantic;
    WasmGen                   m_codegen;
    std::unique_ptr<TreeNode> m_root;
    CompilerOutput            m_output;
};
