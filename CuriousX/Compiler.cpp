// Compiler.cpp
#include "Compiler.hpp"

Compiler::Compiler(std::string_view source, CompilerOutput& output)
    : m_parser(source, output)
    , m_semantic(output)
    , m_codegen(output)
    , m_output(output)
    , m_root(ASTNodeFactory::createTreeNode({}, {"Program", {0, 0}, LexerTokenType::ProgramToken}))
{
}

bool Compiler::compile()
{
    try
    {
        LexerToken token;
        m_parser.advanceToken(token);

        while (token.type != LexerTokenType::Eof)
        {
            if (auto node = m_parser.parseStatement(token))
            {
                processNode(node);
            }
            if (!m_parser.expectNewlineOrEOF(token))
            {
                throw Error("Expected new line before " + std::string(token.value), token.location, ErrorType::SYNTAX);
            }

            m_parser.advancePastNewlines(token);
        }
        collectOutputs();
        return !m_root->children.empty();
    }
    catch (const Error& e)
    {
        m_output.getJson()["error"] = e.what();
        return false;
    }
}

void Compiler::processNode(std::unique_ptr<ASTNode>& node)
{
    m_semantic.analyzeTree(*node);
    m_codegen.generate(*node);
    m_root->children.emplace_back(std::move(node));
}
void Compiler::collectOutputs()
{
    m_parser.addASTToOutput(m_root);
    m_semantic.addSymbolTableToOutput();
    m_codegen.addGeneratedCodeToOutput();
}
