#pragma once
#include <algorithm>
#include <cstddef>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>
#include "helpers/diagnostics.hpp"
#include "helpers/type.hpp"
#include "lexer/token.hpp"
#include "parser/node.hpp"

namespace cx
{
    struct SymbolInfo
    {
        ValueType   valuetype;
        Token       token;
        std::size_t slot; //!< index into the owning function's local list
    };

    //! what is function current condition
    enum class Analysis
    {
        NotStarted,
        InProgress,
        Done,
    };

    struct FunctionInfo
    {
        std::string_view        name;
        Token                   token;
        std::vector<ValueType>  paramTypes;
        ValueType               returnType = ValueType::Unknown;
        std::vector<SymbolInfo> locals;
        Analysis                state = Analysis::NotStarted; // keeps track of functions local var
        FuncNode*               decl  = nullptr;
    };

    struct ActiveFunction
    {
        std::size_t index;
        std::size_t scopeFloor;
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

        void         enterScope()
        {
            m_table.emplace_back();
        }

        void exitScope()
        {
            if (m_table.size() > currentFloor() + 1)
            {
                m_table.pop_back();
            }
        }

        //! Declares `name` if it is not visible, otherwise treats this as an assignment and checks the type matches.
        //! Returns the slot the variable lives in, either way
        std::size_t insert(std::string_view name, ValueType valuetype, Token token)
        {
            if (auto existing = lookup(name))
            {
                if (existing->valuetype != valuetype)
                {
                    m_diag.error(
                        token.location, "cannot assign {} to '{}' of valuetype {}", describe(valuetype), name, describe(existing->valuetype));
                }
                return existing->slot;
            }

            const std::size_t slot = recordLocal(valuetype, token);
            m_table.back().emplace(name, SymbolInfo{valuetype, token, slot});
            return slot;
        }

        //! lookup var names in the table if they exist
        std::optional<SymbolInfo> lookup(std::string_view name) const
        {
            const std::size_t floor = currentFloor();

            // look up within active function range
            for (std::size_t i = m_table.size(); i-- > floor;)
            {
                if (auto it = m_table[i].find(name); it != m_table[i].end())
                {
                    return it->second;
                }
            }
            return std::nullopt;
        }

        //! Functions are global and never nested (i.e declared inside another functions body), so they need no scope stack.
        //! Returns false if `name` is already declared.
        bool declareFunction(std::string_view name, Token token, std::vector<ValueType> paramTypes, ValueType returnType, FuncNode* decl)
        {
            if (const std::size_t existing = findFunction(name); existing != kNoFunction)
            {
                m_diag.error(token.location, "function '{}' is already declared on line {}", name, m_functions[existing].token.location.getLine());
                return false;
            }

            m_functions.push_back(FunctionInfo{name, token, std::move(paramTypes), returnType, {}, Analysis::NotStarted, decl});
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

        FunctionInfo& function(std::size_t index)
        {
            return m_functions[index];
        }

        //! Opens a function's frame: a fresh scope stack floor, and its parameters
        //! inserted as the first locals. `decl` may be null for a synthetic function
        //! (the implicit @main), in which case there are no parameters to insert.
        void beginFunction(std::size_t index)
        {
            m_active.push_back({index, m_table.size()});
            enterScope();

            auto& function = m_functions[index];
            if (function.decl == nullptr)
            {
                return;
            }

            const auto  params = function.decl->parameters;
            const auto& types  = function.paramTypes;

            for (std::size_t i = 0; i < params.size() && i < types.size(); ++i)
            {
                params[i]->valuetype = types[i];
                params[i]->slot      = insert(params[i]->token.value, types[i], params[i]->token);
            }
        }

        void endFunction()
        {
            // drop every scope this function opened
            m_table.resize(m_active.back().scopeFloor);
            m_active.pop_back();
        }

        size_t currentFunction() const
        {
            return m_active.empty() ? kNoFunction : m_active.back().index;
        }

    private:
        //! Appends to the current function's local list and hands back its slot.
        std::size_t recordLocal(ValueType valuetype, Token token)
        {
            const std::size_t index = currentFunction();
            if (index == kNoFunction)
            {
                return 0;
            }

            auto& current = m_functions[index];
            current.locals.push_back(SymbolInfo{valuetype, token, current.locals.size()});

            return current.locals.size() - 1;
        }

        std::size_t currentFloor() const
        {
            return m_active.empty() ? 0 : m_active.back().scopeFloor;
        }

        Table                       m_table;
        Diagnostics&                m_diag;

        std::vector<FunctionInfo>   m_functions;
        std::vector<ActiveFunction> m_active;
    };

} // namespace cx
