#ifndef TREE_HPP
#define TREE_HPP

#include "../utils/Operations.hpp"
#include <memory>
#include <string>

enum class TreeType {
    ST_COMPOUND,
    ST_ASSIGNMENT,
    ST_DEFINITION_TYPE,
    ST_VARIABLE,
    ST_STATEMENT,
    ST_INT,
    ST_STRING,
    ST_NOOP, // No operation
};


struct ASTNode {
    TreeType type;

};

class ASTree {

    private:
      TreeType type;
      List *children;
      std::string name;
      ASTree *value;
      int int_value;
      int data_type;


    public:
        ASTree(TreeType);
        void setName(std::string);
        std::string getName();
        void setValue(ASTree *);
        ASTree *getValue();
        void setDataType(int);
        int getDataType();
        TreeType getType();
        void setType(TreeType);
        List *getChildren() { return this->children; }
        void setIntValue(int);
        int getIntValue() { return this->int_value; };

};


#endif