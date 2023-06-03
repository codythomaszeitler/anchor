#ifndef LEXER_H
#define LEXER_H

#include <string>

namespace lexer
{
    class Location
    {
    private:
        const int row;
        const int column;

    public:
        Location(int, int);

        int getRow();
        int getColumn();

        bool operator==(const Location&) const;
        bool operator!=(const Location&) const;
    };

    enum class TokenType {
        INTEGER_TYPE
    };

    class Token
    {
    private:
        const lexer::TokenType tokenType;
        const std::string raw;
        const lexer::Location start;
        const lexer::Location end;

    public:
        Token(TokenType tokenType, std::string raw, Location start, Location end);
        lexer::TokenType getTokenType() const;        
        std::string getRaw() const;
        lexer::Location getStart() const;
        lexer::Location getEnd() const;
    };

    class Lexer
    {
    private:
        std::string source;

    public:
        Lexer(std::string);
        Token next();
    };
}

#endif