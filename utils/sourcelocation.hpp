#include <string>

class SourceLocation{
  public:
    SourceLocation(unsigned short _line, unsigned short _col): line(_line), col(_col)
    {}

    unsigned getLine(){
      return line;
    }

    unsigned getcol(){
      return col;
    }

    std::string toString() const{
      return "<line:" + std::to_string(line) + ", col:" + std::to_string(col) + ">";
    }
    

  private:
    unsigned short line = 0;
    unsigned short col = 0;
};