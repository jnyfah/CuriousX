#pragma once

namespace cx
{
    enum class ValueType
    {
        Int,
        Float,
        String,
        Bool,
        Void,
        Error,
        Unknown,
    };

    //! for diagnostics: returns char equivalent of the valuetype
    constexpr const char* describe(ValueType t)
    {
        switch (t)
        {
            case ValueType::Int:
                return "integer";
            case ValueType::Float:
                return "float";
            case ValueType::Bool:
                return "boolean";
            case ValueType::String:
                return "string";
            case ValueType::Void:
                return "void";
            case ValueType::Error:
                return "<error>";
            case ValueType::Unknown:
                return "<unknown>";
        }
        return "<error>";
    }

} // namespace cx