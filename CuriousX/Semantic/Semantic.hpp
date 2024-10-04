

#include "Node.hpp"
#include "SymbolTable.hpp"
#include <vector>

class Semantic
{

  public:
    explicit Semantic() : m_symboltable(), flag(false) {}

    bool analyze(const ASTNode& node);
    void analyzeBinaryOperation(const BinaryNode& node);
    void analyzeAssignment(const BinaryNode& node);
    void analyzeExpression(const BinaryNode& node);
    void checkDivisionByZero(const ASTNode& node);
    InferredType inferType(const ASTNode& node);
    InferredType inferTypeFromVariable(const ASTNode& node);
    InferredType inferTypeFromOperation(const BinaryNode& node);
    InferredType inferTypeFromCondition(const BinaryNode& node);
    void analyzeConditionalOperation(const ConditionalNode& node);

  private:
    SymbolTable m_symboltable;
    bool flag;
};

// Binary semantic ??
// print semantic ??
// conditional semantic ??

// check assignment
// check expresion validation
// symbol table to make sure variable has been defined
// no implicit conversion allowed
// if should have actual conditionals in it s expression
//
