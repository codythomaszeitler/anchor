#ifndef LEXER_H
#define LEXER_H

#include <string>

namespace lexer
{
    class Location
    {
    private:
        int row;
        int column;

    public:
        Location(int, int);
        int getRow();

        int getColumn();
    };

    class TokenType
    {
    public:
        static TokenType INTEGER_TYPE;
    };

    class Token
    {
    public:
        Token(TokenType tokenType, std::string raw, Location start, Location end);
        bool operator==(const Token &);
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