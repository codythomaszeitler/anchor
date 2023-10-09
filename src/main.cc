#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Value.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/ArrayRef.h"
#include <memory>

using namespace llvm;

int main(int argc, char *argv[])
{
    std::unique_ptr<LLVMContext> TheContext;
    std::unique_ptr<Module> TheModule;
    std::unique_ptr<IRBuilder<>> Builder;
    TheContext = std::make_unique<LLVMContext>();
    TheModule = std::make_unique<Module>("my cool jit", *TheContext);

    // Create a new builder for the module.
    Builder = std::make_unique<IRBuilder<>>(*TheContext);

    uint64_t cool = 32;
    Value* a = ConstantInt::getIntegerValue(Type::getInt32Ty(*TheContext), APInt(cool, 32));
    Value* b = ConstantInt::getIntegerValue(Type::getInt32Ty(*TheContext), APInt(cool, 32));

    FunctionType* ft = FunctionType::get(Type::getInt32Ty(*TheContext), false);

    Function* theFunction = Function::Create(ft, Function::ExternalLinkage, "foobar", TheModule.get());

    BasicBlock *bb = BasicBlock::Create(*TheContext, "entry", theFunction);
    Builder->SetInsertPoint(bb);
    Value *instruction = Builder->CreateAdd(a, b, "addtmp"); 

    Builder->CreateRet(instruction);

    FunctionType* pft = FunctionType::get(Type::getInt32Ty(*TheContext), true);
    Function* puts = Function::Create(pft, Function::ExternalLinkage, "printf", TheModule.get());

    Value* helloWorldString = Builder->CreateGlobalStringPtr(StringRef("Hello, world!"));
    Value* integerFormatString = Builder->CreateGlobalStringPtr(StringRef("%d"));

    theFunction->print(errs());

    FunctionType* mft = FunctionType::get(Type::getInt32Ty(*TheContext), true);
    Function* main = Function::Create(mft, Function::ExternalLinkage, "main", TheModule.get());

    BasicBlock *mainBB = BasicBlock::Create(*TheContext, "meow", main);
    Builder->SetInsertPoint(mainBB);

    Value* output = Builder->CreateCall(theFunction);

    std::vector<Value*> args;
    args.push_back(integerFormatString);
    args.push_back(output);
    Builder->CreateCall(puts, args);

    Value* c = ConstantInt::getIntegerValue(Type::getInt32Ty(*TheContext), APInt(cool, 32));
    Builder->CreateRet(c);

    TheModule->print(outs(), nullptr);
    return 1;
}