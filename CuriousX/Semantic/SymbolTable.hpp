#pragma once

#include <optional>
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

struct SymbolInfo
{
    InferredType type;
    LexerToken   token;
};

using symbolTable = std::vector<std::unordered_map<std::string, SymbolInfo>>;

class ScopedSymbolTable
{
  private:
    symbolTable scopes;
    int                                                      currentScopeLevel;
    ScopedSymbolTable() : currentScopeLevel(-1)
    {
        enterScope(); // Create global scope
    }
    ScopedSymbolTable(const ScopedSymbolTable&)            = delete;
    ScopedSymbolTable& operator=(const ScopedSymbolTable&) = delete;

  public:
    static ScopedSymbolTable& getInstance()
    {
        static ScopedSymbolTable instance;
        return instance;
    }
    void enterScope()
    {
        scopes.emplace_back();
        currentScopeLevel++;
    }

    void exitScope()
    {
        if (currentScopeLevel > 0)
        {
            scopes.pop_back();
            currentScopeLevel--;
        }
    }

    void insert(const std::string& name, InferredType type, const LexerToken& declarationToken)
    {
        auto& currentScope = scopes[currentScopeLevel];
        if (currentScope.find(name) != currentScope.end())
        {
            throw Error("Unable to infer type", declarationToken.location, ErrorType::SEMANTIC);
        }
        currentScope[name] = SymbolInfo{type, declarationToken};
    }

    bool contains(const std::string& name) const { return lookup(name).has_value(); }

    std::optional<InferredType> lookup(const std::string& name) const
    {
        for (int i = currentScopeLevel; i >= 0; i--)
        {
            auto it = scopes[i].find(name);
            if (it != scopes[i].end())
            {
                return it->second.type;
            }
        }
        return std::nullopt;
    }

    bool remove(const std::string& name)
    {
        auto& currentScope = scopes[currentScopeLevel];
        return currentScope.erase(name) > 0;
    }

    void clear()
    {
        scopes.clear();
        currentScopeLevel = -1;
        enterScope();
    }

    int getCurrentScopeLevel() const { return currentScopeLevel; }

    std::optional<InferredType> lookupCurrentScope(const std::string& name) const
    {
        for (int i = currentScopeLevel; i >= 0; i--)
        {
            auto it = scopes[currentScopeLevel].find(name);
            if (it != scopes[currentScopeLevel].end())
            {
                return it->second.type;
            }
        }
        return std::nullopt;
    }

    bool isFloatType(std::string_view varName) const { return lookup(std::string(varName)) == InferredType::FLOAT; }

    const symbolTable getSymbolTable() { return scopes; }
};