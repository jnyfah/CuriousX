#pragma once
=
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>
#include "helpers/diagnostics.hpp"
#include "helpers/type.hpp"
#include "lexer/token.hpp"

namespace cx
{

    struct SymbolInfo
    {
        ValueType       valuetype;
        Token       token;
        std::size_t slot; //!< index into the owning function's local list which will becomes a frame offset
    };

    struct FunctionInfo
    {
        std::string_view        name;
        Token                   token;
        std::vector<ValueType>       paramTypes;
        ValueType                    returnType = ValueType::Unknown;
        std::vector<SymbolInfo> locals; // keeps track of functions local var
    };

    using Table = std::vector<std::unordered_map<std::string_view, SymbolInfo>>;

    class SymbolTable
    {

    public:
        static constexpr std::size_t kNoFunction = static_cast<std::size_t>(-1);

        explicit SymbolTable(Diagnostics& d) : m_diag(d)
        {
            enterScope(); // global scope
        }

        SymbolTable(const SymbolTable&)            = delete;
        SymbolTable& operator=(const SymbolTable&) = delete;

        void enterScope()
        {
            m_table.emplace_back();
        }

        void exitScope()
        {
            if (m_table.size() > 1)
            {
                m_table.pop_back();
            }
        }

        //! Declares `name` if it is not visible, otherwise treats this as an
        //! assignment and checks the type matches.
        void insert(std::string_view name, ValueType valuetype, Token token)
        {
            if (auto existing = lookup(name))
            {
                if (existing->valuetype != valuetype)
                {
                    m_diag.error(token.location, "cannot assign {} to '{}' of valuetype {}", describe(valuetype), name, describe(existing->valuetype));
                }
                return;
            }

            const std::size_t slot = recordLocal(valuetype, token);
            m_table.back().emplace(name, SymbolInfo{valuetype, token, slot});
        }

        //! lookup var names in the table if they exist
        std::optional<SymbolInfo> lookup(std::string_view name) const
        {
            for (auto scope = m_table.rbegin(); scope != m_table.rend(); ++scope)
            {
                if (auto it = scope->find(name); it != scope->end())
                {
                    return it->second;
                }
            }
            return std::nullopt;
        }

        //! Functions are global and never nested, so they need no scope stack.
        //! Returns false if `name` is already declared.
        bool declareFunction(std::string_view name, Token token, std::vector<ValueType> paramTypes, ValueType returnType)
        {
            if (const std::size_t existing = findFunction(name); existing != kNoFunction)
            {
                m_diag.error(token.location,
                             "function '{}' is already declared on line {}",
                             name,
                             m_functions[existing].token.location.getLine());
                return false;
            }

            m_functions.push_back(FunctionInfo{name, token, std::move(paramTypes), returnType, {}});
            return true;
        }

        //! kNoFunction when there is no such function.
        std::size_t findFunction(std::string_view name) const
        {
            const auto it = std::ranges::find_if(m_functions, [name](const FunctionInfo& f) { return f.name == name; });
            return it == m_functions.end() ? kNoFunction : static_cast<std::size_t>(it - m_functions.begin());
        }

        const std::vector<FunctionInfo>& functions() const
        {
            return m_functions;
        }

        //! Opens `name`'s body: locals recorded from here on belong to it, and its
        //! parameters take the first slots. Returns false if `name` is unknown.
        bool beginFunction(std::string_view name, const std::vector<Token>& paramTokens)
        {
            const std::size_t index = findFunction(name);
            m_currentFunction = index;
            enterScope();

            // insert all the parameters as part of the locals in this scope
            const auto& types = m_functions[index].paramTypes;
            for (std::size_t i = 0; i < paramTokens.size() && i < types.size(); ++i)
            {
                insert(paramTokens[i].value, types[i], paramTokens[i]);
            }
            return true;
        }

        void endFunction()
        {
            exitScope();
            m_currentFunction = kNoFunction;
        }

    private:
        //! Appends to the current function's local list and hands back its slot.
        std::size_t recordLocal(ValueType valuetype, Token token)
        {
            if (m_currentFunction == kNoFunction)
            {
                return 0;
            }

            auto& current = m_functions[m_currentFunction];
            current.locals.push_back(SymbolInfo{valuetype, token, current.locals.size()});

            return current.locals.size() - 1;
        }

        Table                     m_table;
        Diagnostics&              m_diag;

        std::vector<FunctionInfo> m_functions;
        std::size_t               m_currentFunction = kNoFunction;
    };

} // namespace cx
