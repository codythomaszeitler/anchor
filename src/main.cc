#include <iostream>
#include <fstream>
#include <sstream>

#include "llvm/Support/raw_ostream.h"
#include "lexer.hh"
#include "parser.hh"
#include "compiler.hh"

int main(int argc, char *argv[])
{
    std::ifstream t("/Users/codyzeitler/dev/anchor/test.anchor");
    std::stringstream buffer;
    buffer << t.rdbuf();

    parser::Parser parser(lexer::lex(buffer.str()));
    Compiler compiler;

    parser::Program program = parser.parse();
    compiler.compile(llvm::outs(), program);

    return 1;
}