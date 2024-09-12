#pragma once
#include "SourceLocation.hpp"
#include <string>
#include <string_view>
#include <utility>

class Error : public std::exception
{
  public:
    explicit Error(std::string_view message = "An unknown error occurred.") : message(message) {}

    Error(std::string_view message, const SourceLocation& location)
        : message(message.data() + location.toString())
    {
    }

    const char* what() const noexcept override { return message.c_str(); }

  private:
    std::string message;
};
