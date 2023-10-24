#include <iostream>
#include <fstream>
#include <sstream>

#include "llvm/Support/raw_ostream.h"
#include "lexer.hh"
#include "parser.hh"
#include "compiler.hh"

int main(int argc, char *argv[])
{
    std::string input = "";
    if (argc == 1)
    {
        for (std::string line; std::getline(std::cin, line);)
        {
            input += line;
        }
    }
    else
    {
        if (!std::filesystem::exists(argv[1])) 
        {
            std::cout << "Could not find file with name " << argv[1] << std::endl;
            return 1;
        }

        std::ifstream t(argv[1]);
        std::stringstream buffer;
        buffer << t.rdbuf();
        input = buffer.str();
    }

    parser::Parser parser(lexer::lex(input));
    Compiler compiler;

    parser::Program program = parser.parse();

    if (program.isSyntacticallyCorrect())
    {
        compiler.compile(llvm::outs(), program);
    }
    else
    {
        for (parser::ErrorLog errorLog : program.errors)
        {
            std::cout << errorLog.getMessage() << std::endl;
        }
    }

    return 0;
}