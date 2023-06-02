#include "lexer.hh"
#include <stdexcept>

lexer::Location::Location(int row, int column)
{
    if (row < 0)
    {
        throw std::invalid_argument("Cannot construct location with negative row number [" + std::to_string(row) + "].");
    }

    if (column < 0)
    {
        throw std::invalid_argument("Cannot construct location with negative column number [" + std::to_string(column) + "].");
    }

    this->row = row;
    this->column = column;
}

int lexer::Location::getRow()
{
    return this->row;
}

int lexer::Location::getColumn()
{
    return this->column;
}
