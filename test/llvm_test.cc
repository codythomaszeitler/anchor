#include <gtest/gtest.h>
#include <memory>
#include "src/lexer.hh"
#include "src/parser.hh"
#include "src/llvm.hh"

TEST(LlvmTest, ItShouldConvertFunctionIntoLlvm)
{
    std::string sourceCode = "function foo() {return 3 + 5;};";
    std::deque<lexer::Token> tokens = lexer::lex(sourceCode);
    parser::Parser anchorFrontEndParser(tokens);
    parser::Program program = anchorFrontEndParser.parse();

    llvm::Parser testObject(program);
    llvm::Program instructions = testObject.parse();

    std::string functionIdentifier = "foo";
    std::shared_ptr<llvm::Function> llvmFunction = instructions.getFunction(functionIdentifier);
    EXPECT_EQ("foo", llvmFunction->identifier);
    EXPECT_EQ(llvm::Type::I32, llvmFunction->returnType);

    std::shared_ptr<llvm::AddInstruction> addInstruction = llvmFunction->getInstruction<llvm::AddInstruction>(0);

    EXPECT_EQ(3, addInstruction->left);
    EXPECT_EQ(5, addInstruction->right);
    EXPECT_EQ("0", addInstruction->outputIdentifier);
    EXPECT_EQ(llvm::Type::I32, addInstruction->type);

    std::shared_ptr<llvm::RetInstruction> returnInstruction = llvmFunction->getInstruction<llvm::RetInstruction>(1);
    EXPECT_EQ(llvm::Type::I32, returnInstruction->type);
    EXPECT_EQ("0", returnInstruction->inputIdentifier);
}