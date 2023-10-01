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

    if (isdigit(peekChar()))
    {
        return this->parseNumber();
    }

    if (isalpha(peekChar()))
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
    lexer::Location start = lexer::Location(0, this->position);
    std::string raw(1, popChar());

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
    while (isdigit(peekChar()))
    {
        raw += popChar();
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
    while (isalnum(peekChar()))
    {
        raw += popChar();
    }

    lexer::Location end(0, this->position - 1);
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
    else
    {
        return lexer::Token(lexer::TokenType::IDENTIFIER, raw, start, end);
    }
}

char lexer::Lexer::popChar()
{
    char current = this->peekChar();
    this->position++;
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
        popChar();
    }
}
namespace lexer
{
    bool Lexer::hasNext()
    {
        return this->source.length() != this->position;
    }

    std::deque<Token> lex(std::string a)
    {
        lexer::Lexer lexer(a);
        std::deque<Token> tokens;
        while (lexer.hasNext()) {
            tokens.push_back(lexer.next());
        }
        return tokens;
    }
}