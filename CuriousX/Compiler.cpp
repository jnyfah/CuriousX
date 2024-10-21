// Compiler.cpp
#include "Compiler.hpp"

Compiler::Compiler(std::string_view source, CompilerOutput& output)
    : m_parser(source, output)
    , m_semantic()
    , m_codegen()
    , m_root(ASTNodeFactory::createTreeNode({}, {"Program", {0, 0}, LexerTokenType::ProgramToken}))
{
}

bool Compiler::compile()
{
    LexerToken token;
    m_parser.advanceToken(token);

    while (token.type != LexerTokenType::Eof)
    {
        if (auto node = m_parser.parseStatement(token))
        {
            m_semantic.analyze(*node);
            m_root->children.emplace_back(std::move(node));
        }
        if (!m_parser.expectNewlineOrEOF(token))
        {
            throw Error("Expected new line before " + std::string(token.value), token.location, ErrorType::SYNTAX);
        }

        m_parser.advancePastNewlines(token);
    }
    m_codegen.generate(*m_root);
    return !m_root->children.empty();
}


