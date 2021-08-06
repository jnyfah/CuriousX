#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>

class SourceLocation{
  public:
    SourceLocation() = default;
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

//! Represents the start and stop locations for a token, symbol, or expression
class SourceRange {
public:
    SourceRange(SourceLocation start, SourceLocation stop) : start(start), stop(stop) {}
    SourceLocation getStart() const { return start; }
    SourceLocation getEnd() const { return stop; }
private:
    const SourceLocation start = SourceLocation();
    const SourceLocation stop = SourceLocation();
   
};

#endif