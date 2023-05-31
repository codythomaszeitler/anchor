#ifndef LEXER_H
#define LEXER_H

#include <string>

class Lexer
{
private:
    std::string source;

public:
    Lexer(std::string source)
    {
        this->source = source;
    }


};

#endif