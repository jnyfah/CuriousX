#pragma once

#include <algorithm>
#include <optional>
#include <string_view>
#include <unordered_map>

#include "LexerToken.hpp"

class Lexer
{
  public:
    explicit Lexer(std::string_view data) : data(data) {}

    LexerToken nextNWToken()
    {
        LexerToken t;
        do
        {
            t = nextToken();
        } while (t.type == LexerTokenType::Space || t.type == LexerTokenType::Tab);
        return t;
    }

    LexerToken nextToken() { return doGetNextToken(); }

  private:
    std::string_view data;
    size_t           pos   = 0;
    unsigned short   x_pos = 1;
    unsigned short   y_pos = 1;

    struct Checkpoint
    {
        size_t         pos;
        unsigned short x_pos;
        unsigned short y_pos;
    };

    std::optional<Checkpoint> checkpoint;

    void saveCheckpoint() { checkpoint = Checkpoint{pos, x_pos, y_pos}; }

    void restoreCheckpoint()
    {
        if (checkpoint)
        {
            pos   = checkpoint->pos;
            x_pos = checkpoint->x_pos;
            y_pos = checkpoint->y_pos;
            checkpoint.reset();
        }
    }

    char next_char()
    {
        if (pos >= data.size())
            return '\0';
        char c = data[pos++];
        if (c == '\n')
        {
            y_pos++;
            x_pos = 1;
        }
        else
        {
            x_pos++;
        }
        return c;
    }

    char peek_next_char() const { return pos < data.size() ? data[pos] : '\0'; }

    SourceLocation currentLocation() const { return SourceLocation(y_pos, x_pos); }

    static bool isInt(std::string_view data)
    {
        return std::all_of(
            data.begin(), data.end(), [](char c) { return std::isdigit(static_cast<unsigned char>(c)); });
    }

    LexerToken handleComment(size_t startPos, const SourceLocation& location)
    {
        while (true)
        {
            char c = peek_next_char();
            if (c == '\n' || c == '\0')
                break;
            next_char();
        }
        return {data.substr(startPos, pos - startPos), location, LexerTokenType::CommentToken};
    }

    LexerToken doGetNextToken()
    {
        const SourceLocation location = currentLocation();
        const auto           startPos = pos;
        const char           nchar    = next_char();

        // Map of single-character tokens
        static const std::unordered_map<char, std::pair<std::string_view, LexerTokenType>> singleCharTokens = {
            {'\0', {"\0", LexerTokenType::Eof}},
            {'\n', {"\\n", LexerTokenType::Newline}},
            {'\t', {"\t", LexerTokenType::Tab}},
            {'(', {"(", LexerTokenType::ParenOpen}},
            {')', {")", LexerTokenType::ParenClose}},
            {'{', {"{", LexerTokenType::BracesOpen}},
            {'}', {"}", LexerTokenType::BracesClose}},
            {'+', {"+", LexerTokenType::PlusToken}},
            {'/', {"/", LexerTokenType::DivideToken}},
            {'*', {"*", LexerTokenType::MultiplyToken}},
            {'-', {"-", LexerTokenType::MinusToken}}};

        // Check for single-character tokens
        if (auto it = singleCharTokens.find(nchar); it != singleCharTokens.end())
        {
            return {it->second.first, location, it->second.second};
        }

        if (nchar == ' ')
            return {fetch_consecutive(startPos, ' '), location, LexerTokenType::Space};
        if (nchar == '#')
            return handleComment(startPos, location);

        if (nchar == '!')
        {
            if (next_char() == '=')
                return {data.substr(startPos, 2), location, LexerTokenType::NotEqualToken};
            throw Error("Lexical Error- Unexpected character after '!' at line ", location, ErrorType::LEXICAL);
        }

        if (nchar == '>')
        {
            saveCheckpoint();
            if (next_char() == '=')
                return {data.substr(startPos, 2), location, LexerTokenType::GreaterEqualToken};
            restoreCheckpoint();
            return {">", location, LexerTokenType::GreaterToken};
        }

        if (nchar == '<')
        {
            saveCheckpoint();
            if (next_char() == '=')
                return {data.substr(startPos, 2), location, LexerTokenType::LessEqualToken};
            restoreCheckpoint();
            return {"<", location, LexerTokenType::LessToken};
        }

        if (nchar == '=')
        {
            saveCheckpoint();
            if (next_char() == '=')
                return {data.substr(startPos, 2), location, LexerTokenType::EqualToken};
            restoreCheckpoint();
            return {"=", location, LexerTokenType::AssignToken};
        }

        if (nchar == '"')
        {
            size_t count = 1;
            auto   t     = next_char();
            while (t != '"')
            {
                if (t == '\0' || t == '\n')
                    throw Error("Unclosed string literal", location, ErrorType::LEXICAL);
                count++;
                t = next_char();
            }
            return {data.substr(startPos, count + 1), location, LexerTokenType::StringToken};
        }

        if (!(std::isalpha(nchar) || std::isdigit(nchar)))
        {
            throw Error(" Lexical Error- Unknown character at line ", location, ErrorType::LEXICAL);
        }

        // Handle numeric and keyword tokens
        auto substr = next_valid_sequences(startPos);
        if (std::all_of(substr.begin(), substr.end(), [](char c) { return std::isdigit(c) || c == '.'; }))
        {
            return {substr,
                    location,
                    substr.find('.') == std::string::npos ? LexerTokenType::IntToken : LexerTokenType::FloatToken};
        }

        // Map of keyword tokens
        static const std::unordered_map<std::string_view, LexerTokenType> keywords = {
            {"print", LexerTokenType::PrintToken},
            {"Print", LexerTokenType::PrintToken},
            {"if", LexerTokenType::IfToken},
            {"else", LexerTokenType::ElseToken},
            {"true", LexerTokenType::BoolToken},
            {"false", LexerTokenType::BoolToken}};

        if (auto it = keywords.find(substr); it != keywords.end())
        {
            return {substr, location, it->second};
        }

        return {substr, location, LexerTokenType::VarToken};
    }

    // Fetch consecutive occurrences of a character
    std::string_view fetch_consecutive(size_t startPos, char ch)
    {
        while (peek_next_char() == ch)
            next_char();
        return data.substr(startPos, pos - startPos);
    }

    // Fetch next valid sequence of characters (for identifiers and numbers)
    std::string_view next_valid_sequences(size_t from)
    {
        while (std::isalnum(peek_next_char()) || peek_next_char() == '.')
            next_char();
        return data.substr(from, pos - from);
    }
};