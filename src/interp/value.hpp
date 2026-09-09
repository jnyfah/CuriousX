#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include "helpers/type.hpp"

namespace cx
{

    using Value = std::variant<std::monostate, std::int32_t, double, bool, std::string>;

    //! The type an existing Value holds. Void for the result of a call to a
    //! function that returns nothing.
    constexpr ValueType typeOf(const Value& v)
    {
        switch (v.index())
        {
            case 1:
                return ValueType::Int;
            case 2:
                return ValueType::Float;
            case 3:
                return ValueType::Bool;
            case 4:
                return ValueType::String;
            default:
                return ValueType::Void;
        }
    }

    //! A default-constructed Value of `t`, used to size a frame before its
    //! variables are assigned.
    inline Value zeroOf(ValueType t)
    {
        switch (t)
        {
            case ValueType::Int:
                return std::int32_t{0};
            case ValueType::Float:
                return 0.0;
            case ValueType::Bool:
                return false;
            case ValueType::String:
                return std::string{};
            default:
                return std::monostate{};
        }
    }

    //! How `print` renders a value.
    inline std::string toString(const Value& v)
    {
        if (const auto* i = std::get_if<std::int32_t>(&v))
        {
            return std::to_string(*i);
        }
        if (const auto* d = std::get_if<double>(&v))
        {
            std::string s    = std::to_string(*d);

            // trim std::to_string's trailing zeros: 2.500000 -> 2.5, 3.000000 -> 3.0
            const auto  last = s.find_last_not_of('0');
            if (s.find('.') != std::string::npos && last != std::string::npos)
            {
                s.erase(s[last] == '.' ? last + 2 : last + 1);
            }
            return s;
        }
        if (const auto* b = std::get_if<bool>(&v))
        {
            return *b ? "true" : "false";
        }
        if (const auto* s = std::get_if<std::string>(&v))
        {
            return *s;
        }
        return "";
    }

} // namespace cx
