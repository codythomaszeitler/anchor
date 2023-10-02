#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <deque>

#include "src/parser.hh"
#include "src/lexer.hh"
#include "src/llvm.hh"

int main(int argc, char *argv[])
{
    std::string fileName = std::string(argv[1]);

    std::ifstream t("/Users/codyzeitler/dev/anchor/" + fileName);
    std::stringstream buffer;
    buffer << t.rdbuf();

    std::string a = buffer.str();

    std::deque<lexer::Token> tokens = lexer::lex(a);
    parser::Parser frontEnd(tokens);
    parser::Program program = frontEnd.parse();

    llvm::Parser llvmGenerator(program);
    llvm::Program instructions = llvmGenerator.parse();

    llvm::Writer writer;
    std::string output = writer.write(instructions);

    std::ofstream compiledLlvmFile;
    compiledLlvmFile.open("/Users/codyzeitler/dev/anchor/" + fileName + ".ll");
    compiledLlvmFile << output;
    compiledLlvmFile.close();

    return 0;
}