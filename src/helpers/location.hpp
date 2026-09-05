#pragma once
#include <format>
#include <string>

namespace cx
{

    //! Represents the line and column locations for a token, symbol, or expression
    class Location
    {
    public:
        Location() = default;
        Location(unsigned short _line, unsigned short _col) : line(_line), col(_col) {}

        unsigned short getLine() const
        {
            return line;
        }

        unsigned short getCol() const
        {
            return col;
        }

        std::string toString() const
        {
            return std::format("<line:{}, col:{}>", line, col);
        }

    private:
        unsigned short line = 0;
        unsigned short col  = 0;
    };

    //! Represents the start and stop locations for a token, symbol, or expression
    class SourceRange
    {
    public:
        SourceRange(Location start, Location stop) : start(start), stop(stop) {}
        Location getStart() const
        {
            return start;
        }
        Location getEnd() const
        {
            return stop;
        }

    private:
        const Location start;
        const Location stop;
    };

} // namespace cx