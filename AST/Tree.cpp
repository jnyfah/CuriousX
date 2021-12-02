#include "Tree.hpp"

ASTree::ASTree(TreeType type){
    this->type = type;

    if(type == TreeType::ST_COMPOUND){
        this->children = new List(sizeof(ASTree*));
    }
}

void ASTree::setName(std::string name){
    this->name = name;
}

std::string ASTree::getName(){
    return this->name;
}

void ASTree::setValue(ASTree *value){
    this->value = value;
}

ASTree *ASTree::getValue(){
    return this->value;
}

void ASTree::setDataType(int data){
    this->data_type = data;
}

int ASTree::getDataType(){
    return this->data_type;
}

TreeType ASTree::getType(){
    return this->type;
}

void ASTree::setType(TreeType type){
    this->type = type;
}

void ASTree::setIntValue(int int_value){
    this->int_value = int_value;
}