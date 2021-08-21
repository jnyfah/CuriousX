#ifndef ERRORHANDLER_HPP
#define ERRORHANDLER_HPP

#include <string>
#include <exception>



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
    Error() {}
private:
    std::string message;
};

#endif // DIAGNOSTIC_HPP
