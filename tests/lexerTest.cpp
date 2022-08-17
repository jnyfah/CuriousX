#include "gtest/gtest.h"

#include "lexer.hpp"

void LexerEquality(std::vector<LexerToken> actual, std::vector<LexerToken> expected){
    EXPECT_EQ(actual.size(), expected.size());
    for(int i = 0; i < actual.size(); i++){
        EXPECT_EQ(actual[i] , expected[i]);
    }
}