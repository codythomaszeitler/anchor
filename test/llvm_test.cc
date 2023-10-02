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
    EXPECT_EQ("1", addInstruction->outputIdentifier);
    EXPECT_EQ(llvm::Type::I32, addInstruction->type);

    std::shared_ptr<llvm::RetInstruction> returnInstruction = llvmFunction->getInstruction<llvm::RetInstruction>(1);
    EXPECT_EQ(llvm::Type::I32, returnInstruction->type);
    EXPECT_EQ("1", returnInstruction->inputIdentifier);

    std::string expected = "";
    expected.append("define i32 @foo() #0 {\n");
    expected.append("  %1 = add i32 3, 5\n");
    expected.append("  ret i32 %1\n");
    expected.append("}\n");

    llvm::Writer writer;
    std::string output = writer.write(instructions);

    EXPECT_EQ(expected, output);
}

TEST(LlvmTest, ItShouldBeAbleToConvertAnAddInstructionIntoString) 
{
    std::shared_ptr<llvm::AddInstruction> addInstr(new llvm::AddInstruction());
    addInstr->irType = llvm::InstructionType::ADD;
    addInstr->left = 3;
    addInstr->right = 5;
    addInstr->outputIdentifier = "1";
    addInstr->type = llvm::Type::I32;

    llvm::Writer testObject;
    std::string output = testObject.write(addInstr);

    EXPECT_EQ("%1 = add i32 3, 5", output);
}

TEST(LlvmTest, ItShouldBeAbleToConvertARetInstructionIntoString) 
{
    // ret i32 %result
    std::shared_ptr<llvm::RetInstruction> retInstr(new llvm::RetInstruction());
    retInstr->irType = llvm::InstructionType::RET;
    retInstr->type = llvm::Type::I32;
    retInstr->inputIdentifier = "1";

    llvm::Writer testObject;
    std::string output = testObject.write(retInstr);

    EXPECT_EQ("ret i32 %1", output);
}
