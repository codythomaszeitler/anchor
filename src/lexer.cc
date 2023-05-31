#include "lexer.hh"
#include <stdexcept>
#include <cctype>

lexer::Location::Location(int row, int column) : row(row), column(column)
{
    if (row < 0)
    {
        throw std::invalid_argument("Cannot construct location with negative row number [" + std::to_string(row) + "].");
    }

    if (column < 0)
    {
        throw std::invalid_argument("Cannot construct location with negative column number [" + std::to_string(column) + "].");
    }
}

int lexer::Location::getRow()
{
    return this->row;
}

int lexer::Location::getColumn()
{
    return this->column;
}

bool lexer::Location::operator==(const Location &that) const
{
    return this->row == that.row && this->column == that.column;
}

bool lexer::Location::operator!=(const Location &that) const
{
    return !(*this == that);
}

lexer::Token::Token(TokenType tokenType, std::string raw, Location start, Location end)
    : tokenType(tokenType), raw(raw), start(start), end(end)
{
}

lexer::TokenType lexer::Token::getTokenType() const
{
    return this->tokenType;
}

std::string lexer::Token::getRaw() const
{
    return this->raw;
}

lexer::Location lexer::Token::getStart() const
{
    return this->start;
}

lexer::Location lexer::Token::getEnd() const
{
    return this->end;
}

bool lexer::Token::operator==(const Token &that) const
{
    return this->tokenType == that.tokenType &&
           this->raw == that.raw &&
           this->start == that.start &&
           this->end == that.end;
}

lexer::Lexer::Lexer(std::string source)
{
    this->position = 0;
    this->source = source;
}

lexer::Token lexer::Lexer::next()
{
    this->chewUpWhitespace();

    if (peek() == '=')
    {
        return this->parseEquals();
    }

    if (peek() == ';')
    {
        return this->parseSemicolon();
    }

    if (isdigit(peek()))
    {
        return this->parseNumber();
    }

    if (isalpha(peek()))
    {
        return this->parseKeywordOrIdentifier();
    }

    throw std::invalid_argument("Lexer could not parse character at " + std::to_string(this->position));
}

lexer::Token lexer::Lexer::parseEquals()
{
    return parseSingleCharacterTokenType(lexer::TokenType::EQUALS);
}

lexer::Token lexer::Lexer::parseSemicolon()
{
    return parseSingleCharacterTokenType(lexer::TokenType::SEMICOLON);
}

lexer::Token lexer::Lexer::parseSingleCharacterTokenType(lexer::TokenType tokenType)
{
    lexer::Location start = lexer::Location(0, this->position);
    std::string raw(1, pop());

    return lexer::Token(
        tokenType,
        raw,
        start,
        lexer::Location(0, this->position - 1));
}

lexer::Token lexer::Lexer::parseNumber()
{
    std::string raw = "";

    lexer::Location start(0, this->position);
    while (isdigit(peek()))
    {
        raw += pop();
    }

    lexer::Location end(0, this->position - 1);
    return lexer::Token(
        lexer::TokenType::INTEGER,
        raw,
        start,
        end);
}

lexer::Token lexer::Lexer::parseKeywordOrIdentifier()
{
    std::string raw = "";

    lexer::Location start(0, this->position);
    while (isalnum(peek()))
    {
        raw += pop();
    }

    lexer::Location end(0, this->position - 1);
    if ("integer" == raw)
    {
        return lexer::Token(lexer::TokenType::INTEGER_TYPE, raw, start, end);
    }
    else
    {
        return lexer::Token(lexer::TokenType::IDENTIFIER, raw, start, end);
    }
}

char lexer::Lexer::pop()
{
    char current = this->peek();
    this->position++;
    return current;
}

char lexer::Lexer::peek()
{
    return this->source[this->position];
}

void lexer::Lexer::chewUpWhitespace()
{
    while (isspace(peek()))
    {
        pop();
    }
}