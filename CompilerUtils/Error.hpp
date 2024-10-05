#pragma once
#include "SourceLocation.hpp"
#include <string>
#include <string_view>
#include <utility>

enum class ErrorType
{
    SYNTAX,
    SEMANTIC,
    LEXICAL
};

inline std::string_view getErrorTypeDescription(ErrorType t)
{
    switch (t)
    {
    case ErrorType::SYNTAX:
        return "A syntax error occurred";
    case ErrorType::LEXICAL:
        return "A lexical error occurred";
    case ErrorType::SEMANTIC:
        return "A semantic error occurred";
    default:
        return "An unknown error occurred";
    }
}

class Error : public std::exception
{
  public:
    explicit Error(std::string message = "An unknown error occurred.")
        : m_message(std::move(message)), m_type(ErrorType::SYNTAX), m_location()
    {
    }

    Error(std::string message, const SourceLocation& location, ErrorType type = ErrorType::SYNTAX)
        : m_message(std::move(message)), m_type(type), m_location(location)
    {
    }

    const char* what() const noexcept override
    {
        if (m_fullMessage.empty())
        {
            m_fullMessage = getErrorTypeDescription(m_type);
            m_fullMessage += " at " + m_location.toString() + ": " + m_message;
        }
        return m_fullMessage.c_str();
    }

    ErrorType getType() const noexcept { return m_type; }
    const SourceLocation& getLocation() const noexcept { return m_location; }
    const std::string& getMessage() const noexcept { return m_message; }

  private:
    std::string m_message;
    ErrorType m_type;
    SourceLocation m_location;
    mutable std::string m_fullMessage; 
};
