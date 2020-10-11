#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <iomanip>


enum class LexerTokenType {

    ParenOpen,
    ParenClose,
    FloatToken,
    IntToken,
    VarToken,

    PlusToken,
    MinusToken,
    MultiplyToken,
    DivideToken,

    ArrowToken,
    SemiColonToken,
    Space,
    Tab,
    NewLine,
    Eof,

    unknown

};


struct Token {
    std::string value;
    LexerTokenType type;
};

const char* toString(LexerTokenType t);
Token tokenizeCharacter(std::string str);
std::vector<Token>Tokenize(std::string input);

int main() {
    std::string input = "(2-3.87  *-(7 +5.765))";

    std::vector<Token>m_tok = Tokenize(input);

    for (auto x : m_tok) {
        std::cout <<"["<<x.value<<"]"<< '\t'<<toString(x.type)<<std::endl;
    }


}

std::vector<Token>Tokenize(std::string input){
    size_t current  = 0;
    std::string temp;
    std::vector<Token>m_tok;

    while(current < input.length()) {

        if (std::isdigit(input[current])) {
            static const std::regex intRegex{ R"(\d+)"};

            temp = input[current];
            current++;

            while(std::isdigit(input[current]) || input[current] == '.') {
                temp += input[current];
                current++;
            }
            if (std::regex_match(temp, intRegex)) {
                m_tok.push_back({temp, LexerTokenType::IntToken});
            }
            else{
                m_tok.push_back({temp, LexerTokenType::FloatToken});
            }
        }
        else{
            temp = input[current];
            m_tok.push_back(tokenizeCharacter(temp));
            current++;
        }

    }

    return m_tok;

}

Token tokenizeCharacter(std::string str){
    if(str =="("){
        Token m_token{str, LexerTokenType::ParenOpen};
        return m_token;

    } else if(str ==")") {
        Token m_token{str, LexerTokenType::ParenClose};
        return m_token;

    } else if(str =="+") {
       Token m_token{str, LexerTokenType::PlusToken};
        return m_token;

    } else if(str == "-") {
        Token m_token{str, LexerTokenType::MinusToken};
        return m_token;

    } else if(str == "*") {
        Token m_token{str, LexerTokenType::MultiplyToken};
        return m_token;

    } else if(str == "/"){
        Token m_token{str, LexerTokenType::DivideToken};
        return m_token;

    } else if(str == " "){
        Token m_token{str, LexerTokenType::Space};
        return m_token;

    } else if(str == ";") {
        Token m_token{str, LexerTokenType::SemiColonToken};
        return m_token;

    } else if(str == "*") {
        Token m_token{str, LexerTokenType::MultiplyToken};
        return m_token;

    }else if(str == "/") {
        Token m_token{str, LexerTokenType::DivideToken};
        return m_token;

    } else {
        Token m_token{str, LexerTokenType::unknown};
        return m_token;
    }
}

const char* toString(LexerTokenType t){

    switch(t) {
    case LexerTokenType::ParenOpen:
        return "ParenOpen";
    case LexerTokenType::ParenClose:
        return "ParenClose";
    case LexerTokenType::FloatToken:
        return "FloatToken";
    case LexerTokenType::IntToken:
        return "IntToken";
    case LexerTokenType::VarToken:
        return "VarToken";

    case LexerTokenType::PlusToken:
        return "PlusToken";
    case LexerTokenType::MinusToken:
        return "MinusToken";
    case LexerTokenType::MultiplyToken:
        return "MultiplyToken";
    case LexerTokenType::DivideToken:
        return "DivideToken";

    case LexerTokenType::ArrowToken:
        return "ArrowToken";
    case LexerTokenType::SemiColonToken:
        return "SemiColonToken";
    case LexerTokenType::Space:
        return "Space";
    case LexerTokenType::Tab:
        return "Tab";
    case LexerTokenType::NewLine:
        return "NewLine";
    case LexerTokenType::Eof:
        return "Eof";

    case LexerTokenType::unknown:
        return "unknown";
    default:
        break;

    }
    return "<<BAD TYPE>>";
}




