#ifndef SEMANTIC_HPP
#define SEMANTIC_HPP

#include <vector>
#include "SyntaxAnalysis/include/Node.hpp"


class Semantic {

    public:
        Semantic(){}

        void traverse(std::vector<std::unique_ptr<Node>> &compound);


    private:
        

};

#endif