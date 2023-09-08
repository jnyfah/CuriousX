#pragma once
#include <iostream>

#include <exception>
#include <string>
#include <exception>
#include <string>

#include "SourceLocation.hpp"

class Error : public std::exception
{
public:
    explicit Error(const std::string& message = "An unknown error occurred.")
      : message(message)
    {}
    
    Error(const std::string& message, SourceLocation location)
      : message(message + location.toString())
    {}

    const char* what() const noexcept override
    {
        return message.c_str();
    }

private:
    std::string message;
};

// This can be a function outside the class.

