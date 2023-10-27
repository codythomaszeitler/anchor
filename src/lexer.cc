#include "lexer.hh"
#include <stdexcept>
#include <cctype>
#include <map>
#include <iostream>

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

std::string lexer::tostring(lexer::TokenType tokenType)
{
    static std::map<lexer::TokenType, std::string> map;
    if (map.size() == 0)
    {
        map[lexer::TokenType::INTEGER_TYPE] = "INTEGER_TYPE";
        map[lexer::TokenType::INTEGER] = "INTEGER";
        map[lexer::TokenType::IDENTIFIER] = "IDENTIFIER";
        map[lexer::TokenType::VAL] = "VAL";
        map[lexer::TokenType::PLUS_SIGN] = "PLUS_SIGN";
        map[lexer::TokenType::MINUS_SIGN] = "MINUS_SIGN";
        map[lexer::TokenType::MULT_SIGN] = "MULT_SIGN";
        map[lexer::TokenType::EQUALS] = "EQUALS";
        map[lexer::TokenType::SEMICOLON] = "SEMICOLON";
        map[lexer::TokenType::FUNCTION] = "FUNCTION";
        map[lexer::TokenType::LEFT_PAREN] = "LEFT_PAREN";
        map[lexer::TokenType::RIGHT_PAREN] = "RIGHT_PAREN";
        map[lexer::TokenType::LEFT_BRACKET] = "LEFT_BRACKET";
        map[lexer::TokenType::RIGHT_BRACKET] = "RIGHT_BRACKET";
        map[lexer::TokenType::RETURN] = "RETURN";
        map[lexer::TokenType::PRINT] = "PRINT";
        map[lexer::TokenType::STRING] = "STRING";
    }

    return map[tokenType];
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
    this->currentLine = 1;
    this->currentColumn = 1;
    this->source = source;
}

lexer::Token lexer::Lexer::next()
{
    this->chewUpWhitespace();

    if (peekChar() == '=')
    {
        return this->parseEquals();
    }

    if (peekChar() == ';')
    {
        return this->parseSemicolon();
    }

    if (peekChar() == '(')
    {
        return this->parseLeftParen();
    }

    if (peekChar() == ')')
    {
        return this->parseRightParen();
    }

    if (peekChar() == '{')
    {
        return this->parseLeftBracket();
    }

    if (peekChar() == '}')
    {
        return this->parseRightBracket();
    }

    if (peekChar() == '+')
    {
        return parseSingleCharacterTokenType(lexer::TokenType::PLUS_SIGN);
    }

    if (peekChar() == '-')
    {
        return parseSingleCharacterTokenType(lexer::TokenType::MINUS_SIGN);
    }

    if (peekChar() == '*')
    {
        return parseSingleCharacterTokenType(lexer::TokenType::MULT_SIGN);
    }

    if (peekChar() == '"')
    {
        return this->parseRawStringLiteral();
    }

    if (peekChar() == ',')
    {
        return parseSingleCharacterTokenType(lexer::TokenType::SEMICOLON);
    }

    if (peekChar() == '<')
    {
        return parseSingleCharacterTokenType(lexer::TokenType::LESS_THAN_SIGN);
    }

    if (isdigit(peekChar()))
    {
        return this->parseNumber();
    }

    if (isalpha(peekChar()))
    {
        return this->parseKeywordOrIdentifier();
    }

    if (peekChar() == '\0')
    {
        lexer::Location start(this->currentLine, this->currentColumn);
        char endOfString = popChar();
        return lexer::Token(
            lexer::TokenType::END_OF_STREAM,
            "\0",
            start,
            lexer::Location(this->currentLine, this->currentColumn - 1));
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

lexer::Token lexer::Lexer::parseLeftParen()
{
    return parseSingleCharacterTokenType(lexer::TokenType::LEFT_PAREN);
}

lexer::Token lexer::Lexer::parseRightParen()
{
    return parseSingleCharacterTokenType(lexer::TokenType::RIGHT_PAREN);
}

lexer::Token lexer::Lexer::parseLeftBracket()
{
    return parseSingleCharacterTokenType(lexer::TokenType::LEFT_BRACKET);
}

lexer::Token lexer::Lexer::parseRightBracket()
{
    return parseSingleCharacterTokenType(lexer::TokenType::RIGHT_BRACKET);
}

lexer::Token lexer::Lexer::parseSingleCharacterTokenType(lexer::TokenType tokenType)
{
    lexer::Location start(this->currentLine, this->currentColumn);
    std::string raw(1, popChar());

    return lexer::Token(
        tokenType,
        raw,
        start,
        lexer::Location(this->currentLine, this->currentColumn - 1));
}

lexer::Token lexer::Lexer::parseNumber()
{
    std::string raw = "";

    lexer::Location start(this->currentLine, this->currentColumn);
    while (isdigit(peekChar()))
    {
        raw += popChar();
    }

    lexer::Location end(this->currentLine, this->currentColumn - 1);
    return lexer::Token(
        lexer::TokenType::INTEGER,
        raw,
        start,
        end);
}

lexer::Token lexer::Lexer::parseRawStringLiteral()
{
    lexer::Location start(this->currentLine, this->currentColumn);

    std::string raw(1, this->popChar());
    while (this->peekChar() != '"')
    {
        raw += this->popChar();
    }
    // should be a ""
    raw.push_back(this->popChar());

    lexer::Location end(this->currentLine, this->currentColumn - 1);
    return lexer::Token(
        lexer::TokenType::STRING,
        raw,
        start,
        end);
}

lexer::Token lexer::Lexer::parseKeywordOrIdentifier()
{
    std::string raw = "";

    lexer::Location start(this->currentLine, this->currentColumn);
    while (isalnum(peekChar()))
    {
        raw += popChar();
    }

    lexer::Location end(this->currentLine, this->currentColumn - 1);
    if ("val" == raw)
    {
        return lexer::Token(lexer::TokenType::VAL, raw, start, end);
    }
    else if ("return" == raw)
    {
        return lexer::Token(lexer::TokenType::RETURN, raw, start, end);
    }
    else if ("function" == raw)
    {
        return lexer::Token(lexer::TokenType::FUNCTION, raw, start, end);
    }
    else if ("print" == raw)
    {
        return lexer::Token(lexer::TokenType::PRINT, raw, start, end);
    }
    else if ("integer" == raw)
    {
        return lexer::Token(lexer::TokenType::INTEGER_TYPE, raw, start, end);
    }
    else if ("boolean" == raw)
    {
        return lexer::Token(lexer::TokenType::BOOLEAN_TYPE, raw, start, end);
    }
    else if ("void" == raw)
    {
        return lexer::Token(lexer::TokenType::VOID_TYPE, raw, start, end);
    }
    else if ("true" == raw)
    {
        return lexer::Token(lexer::TokenType::TRUE, raw, start, end);
    }
    else if ("false" == raw)
    {
        return lexer::Token(lexer::TokenType::FALSE, raw, start, end);
    }
    else
    {
        return lexer::Token(lexer::TokenType::IDENTIFIER, raw, start, end);
    }
}

char lexer::Lexer::popChar()
{
    char current = this->peekChar();
    this->position++;
    this->currentColumn++;
    return current;
}

char lexer::Lexer::peekChar()
{
    return this->source[this->position];
}

void lexer::Lexer::chewUpWhitespace()
{
    while (isspace(peekChar()))
    {
        if (peekChar() == '\n')
        {
            this->currentLine++;
            this->currentColumn = 0;
        }

        popChar();
    }
}
namespace lexer
{
    bool Lexer::hasNext()
    {
        const int lengthIncludingNullTerminator = this->source.length() + 1;
        return lengthIncludingNullTerminator != this->position;
    }

    std::deque<Token> lex(std::string a)
    {
        lexer::Lexer lexer(a);
        std::deque<Token> tokens;
        while (lexer.hasNext())
        {
            tokens.push_back(lexer.next());
        }
        return tokens;
    }
}