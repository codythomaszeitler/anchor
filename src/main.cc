#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <memory>
#include "llvm/IR/Constants.h"
#include "llvm/IR/Value.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/StringRef.h"

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

    Value* a = ConstantFP::get(*TheContext, APFloat(2.0));
    Value* b = ConstantFP::get(*TheContext, APFloat(2.0));

    FunctionType* ft = FunctionType::get(Type::getDoubleTy(*TheContext), false);

    Function* theFunction = Function::Create(ft, Function::ExternalLinkage, "foobar", TheModule.get());

    BasicBlock *bb = BasicBlock::Create(*TheContext, "entry", theFunction);
    Builder->SetInsertPoint(bb);
    Value *instruction = Builder->CreateFAdd(a, b, "addtmp");

    Builder->CreateRet(instruction);


    theFunction->print(errs());

    Function* main = Function::Create(ft, Function::ExternalLinkage, "_main", TheModule.get());
    BasicBlock *mainBB = BasicBlock::Create(*TheContext, "meow", main);
    Builder->SetInsertPoint(mainBB);

    Builder->CreateCall(theFunction);

    TheModule->print(errs(), nullptr);
    return 1;
}