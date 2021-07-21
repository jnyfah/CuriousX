#include "tokenizer.hpp"

#include <iomanip>
#include <iostream>



bool Lexer::IsEndOfLine(char c) {
     bool result = false;

    if ((c == '\n') || (c == '\r')) {
         result = true;
    }

    return result;
	
}

bool Lexer::IsWhiteSpace(char c) {
    bool result = false;

    if ((c == ' ') || (c == '\t') || (c == '\f') || (c == '\v')) {
        result = true;
    }
	return result;
}

bool Lexer::IsLetter(char c) {
    bool result = false;

	if ((c >= 'A') && (c <= 'Z')) result = true;
	if ((c >= 'a') && (c <= 'z')) result = true;

	return result;
}

bool Lexer::IsNumeric(char c) {
    bool result = false;

	if ((c >= '0') && (c <= '9')){
        result = true;
    } 

	return result;
}



Token Lexer::tokenizeCharacter(std::string str, unsigned short row, unsigned short col ){
    if(str =="("){
        Token token{str, LexerTokenType::ParenOpen, {row,col}};
        return token;

    } else if(str ==")") {
        Token token{str, LexerTokenType::ParenClose, {row,col}};
        return token;

    } else if(str =="+") {
       Token token{str, LexerTokenType::PlusToken, {row,col}};
        return token;

    } else if(str == "-") {
        Token token{str, LexerTokenType::MinusToken, {row,col}};
        return token;

    } else if(str == "*") {
        Token token{str, LexerTokenType::MultiplyToken, {row,col}};
        return token;

    } else if(str == "/"){
        Token token{str, LexerTokenType::DivideToken,{row,col}};
        return token;

    } else if(str == ":") {
        Token token{str, LexerTokenType::ColonToken, {row,col}};
        return token;

    } else if(str == "*") {
        Token token{str, LexerTokenType::MultiplyToken, {row,col}};
        return token;

    } else if(str == "=") {
        Token token{str, LexerTokenType::EqualToken, {row,col}};
        return token;

    }else if(str == "/") {
        Token token{str, LexerTokenType::DivideToken, {row,col}};
        return token;

    } else {
        Token token{str, LexerTokenType::unknown, {row,col}};
        return token;
    }
}

const char* Lexer::toString(LexerTokenType t){

    switch(t) {
    case LexerTokenType::ParenOpen:           return "ParenOpen";
   
    case LexerTokenType::ParenClose:          return "ParenClose";
   
    case LexerTokenType::FloatToken:          return "FloatToken";
   
    case LexerTokenType::IntToken:            return "IntToken";
   
    case LexerTokenType::VarToken:            return "VarToken";

    case LexerTokenType::PlusToken:           return "PlusToken";
   
    case LexerTokenType::MinusToken:          return "MinusToken";
    
    case LexerTokenType::MultiplyToken:       return "MultiplyToken";
    
    case LexerTokenType::DivideToken:         return "DivideToken";

    case LexerTokenType::ArrowToken:          return "ArrowToken";
   
    case LexerTokenType::ColonToken:      return "ColonToken";
   
    case LexerTokenType::Space:               return "Space";
   
    case LexerTokenType::Tab:                 return "Tab";
   
    case LexerTokenType::NewLine:             return "NewLine";
  
    case LexerTokenType::Eof:                 return "Eof";

    case LexerTokenType::print:               return "PrintToken";

    case LexerTokenType::unknown:             return "unknown";
   
    default:                                  break;

    }
    return "<<BAD TYPE>>";
}






std::vector<Token> Lexer::Tokenize(std::string input, unsigned short line){
    unsigned short current  = 0;
    unsigned short row =line;
    unsigned short col = 0;
    std::string temp = "";
    
    std::vector<Token>token;

    while(current < input.length()) {

       if (this->IsNumeric(input[current])) {
            temp = input[current];
            col = current;
            current++;

            while(this->IsNumeric(input[current]) || input[current] == '.') {
                temp += input[current];
                current++;
            }
            if (temp.find('.') == std::string::npos) {
                token.push_back({temp, LexerTokenType::IntToken, {row,col}});
            }
            else{
                token.push_back({temp, LexerTokenType::FloatToken, {row,col}});
            }
        } else if(this->IsLetter(input[current])){
            temp = input[current];
            col = current;
            current++;   
            while(this->IsLetter(input[current]) || input[current] == '_') {

                temp += input[current];
                current++;
            }
            if(temp == "print") {
                token.push_back({temp, LexerTokenType::print, {row,col}});
            } else {
                token.push_back({temp, LexerTokenType::VarToken, {row,col}});
            }
        } else if(this->IsWhiteSpace(input[current])){
            temp = input[current];
            col = current;
            current++;   
            while(this->IsWhiteSpace(input[current])) {

                temp += input[current];
                current++;
            }
             token.push_back({temp, LexerTokenType::Space, {row,col}});
            
        } else{
            temp = input[current];
            if((input[current] == '-') && (input[current ++] == '>')){
                token.push_back({"->", LexerTokenType::ArrowToken, {row, current}});
                current+=2;
            }else{
               token.push_back(tokenizeCharacter(temp, row, current));
               current++;
            }
            
        }

    }

    return token;
}