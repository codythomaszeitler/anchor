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

namespace compiler {
    class Compiler {
    
    private:
        std::unique_ptr<llvm::LLVMContext> context;
        std::unique_ptr<llvm::Module> compiling; 
        std::unique_ptr<llvm::IRBuilder<>> builder;
        void compile(llvm::raw_ostream &outs, std::shared_ptr<parser::FunctionStmt> functionStmt);
        void compile(llvm::raw_ostream &outs, std::shared_ptr<parser::PrintStmt> printStmt);
        void compile(llvm::raw_ostream &outs, std::shared_ptr<parser::ReturnStmt> returnStmt);
        void compile(llvm::raw_ostream &outs, std::shared_ptr<parser::Stmt> stmt);
        llvm::Value* compile(llvm::raw_ostream &outs, std::shared_ptr<parser::Expr> expr);
        llvm::Value* compile(llvm::raw_ostream &outs, std::shared_ptr<parser::StringLiteral> stringLiteral);
        llvm::Value* compile(llvm::raw_ostream &outs, std::shared_ptr<parser::IntegerLiteral> integerLiteral);
    
        using Body = std::vector<std::shared_ptr<parser::Stmt>>;
        void compile(llvm::raw_ostream &outs, Body functionStmt);
    
        void declarePrintFunction();
    public:
        Compiler();
        void compile(llvm::raw_ostream&, parser::Program);
    };

}
#endif 
