/****
 **
 ** @copyright copyright (c) 2022
 **
 **
 ** Distributed under the Boost Software License, Version 1.0.
 ** (See accompanying file LICENSE_1_0.txt or copy at
 ** http://www.boost.org/LICENSE_1_0.txt)
 **
 **
 ** @author Jennifer Chukwu
 ** @email: <jnyfaah@gmail.com>
 **
 ** see https://github.com/jnyfah/CuriousX for most recent version including documentation.
 ** Project CuriousX...2022
 **
 */

#ifndef _SOURCE_LOCATION_HPP_
#define _SOURCE_LOCATION_HPP_

///////////////////////////////////////////////////////////////////////////
/// This file contains implementation of the helper logic for tracking source locations
/// as it is useful for the parser or typechecker to know exactly what line
/// and column number a token was located at
///////////////////////////////////////////////////////////////////////////

#include <string>

class SourceLocation
{
  public:
    SourceLocation() = default;
    SourceLocation(unsigned short _line, unsigned short _col)
      : line(_line)
      , col(_col)
    {}

    unsigned getLine()
    {
        return line;
    }

    unsigned getCol()
    {
        return col;
    }

    std::string toString() const
    {
        return "<line:" + std::to_string(line) + ", col:" + std::to_string(col) + ">";
    }


  private:
    unsigned short line = 0;
    unsigned short col = 0;
};

//! Represents the start and stop locations for a token, symbol, or expression
class SourceRange
{
  public:
    SourceRange(SourceLocation start, SourceLocation stop)
      : start(start)
      , stop(stop)
    {}
    SourceLocation getStart() const
    {
        return start;
    }
    SourceLocation getEnd() const
    {
        return stop;
    }

  private:
    const SourceLocation start = SourceLocation();
    const SourceLocation stop = SourceLocation();
};

#endif