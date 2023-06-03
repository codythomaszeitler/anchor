#include "lexer.hh"
#include <stdexcept>

lexer::Location::Location(int row, int column) : row(row), column(column)
{
    if (row <= 0)
    {
        throw std::invalid_argument("Cannot construct location with non-positive row number [" + std::to_string(row) + "].");
    }

    if (column <= 0)
    {
        throw std::invalid_argument("Cannot construct location with non-positive column number [" + std::to_string(column) + "].");
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
