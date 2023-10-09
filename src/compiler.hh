#ifndef __COMPILER_H__
#define __COMPILER_H__

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Value.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/raw_ostream.h"
#include <memory>
#include "src/parser.hh"

class Compiler {

private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> compiling; 
    std::unique_ptr<llvm::IRBuilder<>> builder;

public:
    Compiler();
    void compile(llvm::raw_ostream&, parser::Program);
};

#endif 