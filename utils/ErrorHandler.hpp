#ifndef ERRORHANDLER_HPP
#define ERRORHANDLER_HPP

#include <string>
#include <exception>
#include <stdexcept>

#include "SourceLocation.hpp"



class Diagnostic {
    void addWarning(std::string msg);
    void addError(std::string msg);
    bool hasErrors;
private:
    //class Impl;
    //Impl* impl;
};


class Error {
public:
    Error(){}

    void missingNumber(std::string message, SourceLocation location) {
        throw std::invalid_argument(message + location.toString());
    }


private:
   
};

#endif // DIAGNOSTIC_HPP
