#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "Node.hpp"

enum class InferredType
{
    INTEGER,
    FLOAT,
    STRING,
    BOOL
};

struct Symbol
{
    InferredType type;
    LexerToken token;

    Symbol(InferredType t, const LexerToken& tok) : type(t), token(tok) {}
};

class SymbolTable
{
  private:
    std::unordered_map<std::string, Symbol> symbols;

  public:
    void insert(const std::string& name, InferredType type, const LexerToken& token)
    {
        auto [it, inserted] = symbols.try_emplace(name, Symbol(type, token));
        if (!inserted)
        {
            throw ("Error: Variable '" + name + "' already defined at " +
                                     token.location.toString());
        }
    }

    bool contains(const std::string& name) const { return symbols.find(name) != symbols.end(); }

    std::optional<InferredType> getType(const std::string& name) const
    {
        auto it = symbols.find(name);
        if (it != symbols.end())
        {
            return it->second.type;
        }
        return std::nullopt;
    }

    std::optional<LexerToken> getToken(const std::string& name) const
    {
        auto it = symbols.find(name);
        if (it != symbols.end())
        {
            return it->second.token;
        }
        return std::nullopt;
    }
};