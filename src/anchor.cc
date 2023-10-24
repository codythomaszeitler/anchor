#include "src/anchor.hh"

#include "src/lexer.hh"
#include "src/parser.hh"
#include "src/compiler.hh"
#include "llvm/Support/raw_ostream.h"

namespace anchor
{
    std::string compile(std::string input)
    {
        parser::Parser parser(lexer::lex(input));
        compiler::Compiler compiler;

        parser::Program program = parser.parse();

        if (program.isSyntacticallyCorrect())
        {
            std::string llvmOutputRef;
            llvm::raw_string_ostream llvmOutput(llvmOutputRef);

            compiler.compile(llvmOutput, program);

            return llvmOutputRef;
        }
        else
        {
            std::string errors = "";
            for (parser::ErrorLog errorLog : program.errors)
            {
                errors += errorLog.getMessage() + "\n";
            }
            return errors;
        }
    }
}