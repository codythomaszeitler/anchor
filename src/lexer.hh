#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <map>

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

        bool operator==(const Location &) const;
        bool operator!=(const Location &) const;
    };

    enum class TokenType
    {
        INTEGER_TYPE,
        INTEGER,
        IDENTIFIER,
        VAL,
        PLUS_SIGN,
        EQUALS,
        SEMICOLON,
        FUNCTION,
        LEFT_PAREN,
        RIGHT_PAREN,
        LEFT_BRACKET,
        RIGHT_BRACKET,
        RETURN
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

        bool operator==(const Token &) const;
    };

    class Lexer
    {
    private:
        int position;
        std::string source;

        void chewUpWhitespace();

        lexer::Token parseKeywordOrIdentifier();
        lexer::Token parseEquals();
        lexer::Token parseSemicolon();
        lexer::Token parseLeftParen();
        lexer::Token parseRightParen();
        lexer::Token parseLeftBracket();
        lexer::Token parseRightBracket();
        lexer::Token parseSingleCharacterTokenType(lexer::TokenType tokenType);
        lexer::Token parseNumber();

        char pop();
        char peek();

    public:
        Lexer(std::string);
        lexer::Token next();
    };
}

#endif