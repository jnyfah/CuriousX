#include "interp.hpp"
#include <charconv>
#include <cstdint>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include "helpers/type.hpp"
#include "interp/value.hpp"
#include "lexer/token.hpp"
#include "parser/node.hpp"
#include "sema/sema.hpp"

namespace cx
{
    namespace
    {
        std::int32_t wrapping(std::int32_t a, std::int32_t b, TokenType op)
        {
            const auto x = static_cast<std::uint32_t>(a);
            const auto y = static_cast<std::uint32_t>(b);

            switch (op)
            {
                case TokenType::Plus:
                    return static_cast<std::int32_t>(x + y);
                case TokenType::Minus:
                    return static_cast<std::int32_t>(x - y);
                case TokenType::Multiply:
                    return static_cast<std::int32_t>(x * y);
                default:
                    return 0;
            }
        }
    } // namespace

    Interpreter::Interpreter(const SymbolTable& sym) : m_sym(sym) {}

    void Interpreter::run(const ProgramNode& root)
    {
        auto index = m_sym.findFunction("@main");
        m_frames.emplace_back(m_sym.functions()[index].locals.size());

        for (Node* statement : root.statements)
        {
            eval(statement);
        }
        m_frames.pop_back();
    }

    void Interpreter::eval(const Node* node)
    {
        switch (node->kind)
        {
            case NodeKind::IfStmt:
                evalIf(static_cast<const IfNode*>(node));
                break;

            case NodeKind::WhileStmt:
                evalWhile(static_cast<const WhileNode*>(node));
                break;

            case NodeKind::Print:
                evalPrint(static_cast<const PrintNode*>(node));
                break;

            case NodeKind::ReturnStmt:
            {
                // unwinds out of any nested blocks to the catch in evalCall
                const auto* r = static_cast<const ReturnNode*>(node);
                throw ReturnSignal{r->expression ? evalExpr(r->expression) : Value{std::monostate{}}};
            }

            case NodeKind::FuncDecl:
                // a declaration does nothing at run time; the body runs when called
                break;

            default:
                evalExpr(node);
                break;
        }
    }

    Value Interpreter::evalExpr(const Node* node)
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
            case cx::NodeKind::Identifier:
                return m_frames.back().slots[node->localIndex];
            case NodeKind::String:
                return std::string{node->token.value};
            case NodeKind::FuncCall:
                return evalCall(static_cast<const CallNode*>(node));
            case NodeKind::BinaryExpr:
                return evalBinary(static_cast<const BinaryNode*>(node));
            case cx::NodeKind::UnaryExpr:
                return evalUnary(static_cast<const UnaryNode*>(node));
            default:
                return std::monostate{};
        }
    }

    Value Interpreter::evalBinary(const BinaryNode* node)
    {
        const TokenType op = node->token.type;

        // assignment is not a computation: evaluate the right side, then store it
        if (op == TokenType::Assign)
        {
            Value v                                       = evalExpr(node->right);
            m_frames.back().slots[node->left->localIndex] = v;
            return v;
        }

        const Value left  = evalExpr(node->left);
        const Value right = evalExpr(node->right);

        // Sema guarantees the operands agree, so one side decides how to read both
        switch (typeOf(left))
        {
            case ValueType::Int:
            {
                const auto a = std::get<std::int32_t>(left);
                const auto b = std::get<std::int32_t>(right);

                switch (op)
                {
                    case TokenType::Plus:
                    case TokenType::Minus:
                    case TokenType::Multiply:
                        return wrapping(a, b, op);
                        // avoid divide by 0 UB
                    case TokenType::Divide:
                        return b == 0 ? std::int32_t{0} : static_cast<std::int32_t>(a / b);
                    case TokenType::Percent:
                        return b == 0 ? std::int32_t{0} : static_cast<std::int32_t>(a % b);

                    case TokenType::Equal:
                        return a == b;
                    case TokenType::NotEqual:
                        return a != b;
                    case TokenType::Less:
                        return a < b;
                    case TokenType::LessEqual:
                        return a <= b;
                    case TokenType::Greater:
                        return a > b;
                    case TokenType::GreaterEqual:
                        return a >= b;
                    default:
                        return std::monostate{};
                }
            }

            case ValueType::Float:
            {
                const auto a = std::get<double>(left);
                const auto b = std::get<double>(right);

                switch (op)
                {
                    case TokenType::Plus:
                        return a + b;
                    case TokenType::Minus:
                        return a - b;
                    case TokenType::Multiply:
                        return a * b;
                    case TokenType::Divide:
                        return a / b; // IEEE: produces inf/nan rather than UB

                    case TokenType::Equal:
                        return a == b;
                    case TokenType::NotEqual:
                        return a != b;
                    case TokenType::Less:
                        return a < b;
                    case TokenType::LessEqual:
                        return a <= b;
                    case TokenType::Greater:
                        return a > b;
                    case TokenType::GreaterEqual:
                        return a >= b;
                    default:
                        return std::monostate{};
                }
            }

            case ValueType::Bool:
            {
                const auto a = std::get<bool>(left);
                const auto b = std::get<bool>(right);

                switch (op)
                {
                    case TokenType::And:
                        return a && b;
                    case TokenType::Or:
                        return a || b;
                    case TokenType::Equal:
                        return a == b;
                    case TokenType::NotEqual:
                        return a != b;
                    default:
                        return std::monostate{};
                }
            }

            case ValueType::String:
            {
                const auto& a = std::get<std::string>(left);
                const auto& b = std::get<std::string>(right);

                switch (op)
                {
                    case TokenType::Equal:
                        return a == b;
                    case TokenType::NotEqual:
                        return a != b;
                    default:
                        return std::monostate{};
                }
            }

            default:
                return std::monostate{};
        }
    }

    Value Interpreter::evalCall(const CallNode* node)
    {
        auto               index = m_sym.findFunction(node->callee->token.value);

        // the parameters can be part of @main body, if so extract from m_frame and put it in args
        std::vector<Value> args;
        args.reserve(node->arguments.size());
        for (auto n : node->arguments)
        {
            args.push_back(evalExpr(n));
        }

        m_frames.emplace_back(m_sym.functions()[index].locals.size());

        // parameters are also part of local function body and take first slots
        for (size_t i = 0; i < args.size(); i++)
        {
            m_frames.back().slots[i] = std::move(args[i]);
        }

        Value result = std::monostate{};
        try
        {
            auto funcnode = m_sym.functions()[index].decl;
            for (Node* statement : funcnode->body)
            {
                eval(statement);
            }
        }
        catch (ReturnSignal& r)
        {
            result = std::move(r.value);
        }

        m_frames.pop_back();
        return result;
    }

    void Interpreter::evalIf(const IfNode* node)
    {
        // Sema has already checked the condition is a boolean
        const bool taken = std::get<bool>(evalExpr(node->condition));

        // `else if` was parsed as a nested IfNode sitting alone in nelse,
        // so a chain needs no special handling here
        for (const Node* statement : taken ? node->then : node->nelse)
        {
            eval(statement);
        }
    }

    void Interpreter::evalWhile(const WhileNode* node)
    {
        // re-evaluated every iteration
        while (std::get<bool>(evalExpr(node->condition)))
        {
            for (const Node* statement : node->loop)
            {
                eval(statement);
            }
        }
    }

    Value Interpreter::evalUnary(const UnaryNode* node)
    {
        const Value operand = evalExpr(node->operand);

        switch (node->token.type)
        {
            case TokenType::Minus:
                if (const auto* d = std::get_if<double>(&operand))
                {
                    return -*d;
                }
                // negation of INT32_MIN overflows, so go through unsigned as elsewhere
                return static_cast<std::int32_t>(0U - static_cast<std::uint32_t>(std::get<std::int32_t>(operand)));

            case TokenType::Not:
                return !std::get<bool>(operand);

            default:
                return std::monostate{};
        }
    }

    void Interpreter::evalPrint(const PrintNode* node)
    {
        m_output += toString(evalExpr(node->expression));
        m_output += '\n';
    }

    const std::string& Interpreter::output() const
    {
        return m_output;
    }

} // namespace cx