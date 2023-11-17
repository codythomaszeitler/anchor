#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <map>
#include <vector>
#include <deque>

namespace lexer
{
    class Location
    {
    private:
        const int row;
        const int column;

    public:
        Location(int, int);

        int getRow() const;
        int getColumn() const;

        bool operator==(const Location &) const;
        bool operator!=(const Location &) const;
    };

    enum class TokenType
    {
        INTEGER_TYPE,
        BOOLEAN_TYPE,
        STRING_TYPE,
        VOID_TYPE,
        INTEGER,
        IDENTIFIER,
        VAL,
        PLUS_SIGN,
        MINUS_SIGN,
        MULT_SIGN,
        LESS_THAN_SIGN,
        GREATER_THAN_SIGN,
        EQUALS,
        DOUBLE_EQUALS,
        SEMICOLON,
        FUNCTION,
        LEFT_PAREN,
        RIGHT_PAREN,
        LEFT_BRACKET,
        RIGHT_BRACKET,
        RETURN,
        PRINT,
        STRING,
        END_OF_STREAM,
        TRUE,
        FALSE,
        IF,
        WHILE
    };

    std::string tostring(lexer::TokenType tokenType);

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
        int currentLine;
        int currentColumn;
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
        lexer::Token parseRawStringLiteral();

        char popChar();
        char peekChar();

    public:
        Lexer(std::string);
        lexer::Token next();
        bool hasNext();
    };

    std::deque<lexer::Token> lex(std::string);
}


#endif