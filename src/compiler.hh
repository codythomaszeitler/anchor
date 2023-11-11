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

        llvm::StructType* anchorStringStructType;

        void compile(llvm::raw_ostream &outs, std::shared_ptr<parser::FunctionStmt> functionStmt);
        llvm::Function* getFunctionWithNamedParams(std::shared_ptr<parser::FunctionStmt> functionStmt);
        llvm::FunctionType* functionType(std::shared_ptr<parser::FunctionStmt> functionStmt);
        std::vector<llvm::Type*> functionStmtArgTypes(std::vector<std::shared_ptr<parser::FunctionArgStmt>> args);

        void compile(llvm::raw_ostream &outs, std::shared_ptr<parser::PrintStmt> printStmt);
        void compile(llvm::raw_ostream &outs, std::shared_ptr<parser::ReturnStmt> returnStmt);
        void compile(llvm::raw_ostream &outs, std::shared_ptr<parser::VarDeclStmt> varDeclStmt);
        void compile(llvm::raw_ostream &outs, std::shared_ptr<parser::Stmt> stmt);
        void compile(llvm::raw_ostream &outs, std::shared_ptr<parser::IfStmt> ifStmt);
        void compile(llvm::raw_ostream &outs, std::shared_ptr<parser::WhileStmt> ifStmt);
        void compile(llvm::raw_ostream &outs, std::shared_ptr<parser::ExprStmt> exprStmt);

        llvm::Value* compile(llvm::raw_ostream &outs, std::shared_ptr<parser::Expr> expr);
        llvm::Value* compile(llvm::raw_ostream &outs, std::shared_ptr<parser::StringLiteral> stringLiteral);
        llvm::Value* compile(llvm::raw_ostream &outs, std::shared_ptr<parser::IntegerLiteral> integerLiteral);
        llvm::Value* compile(llvm::raw_ostream &outs, std::shared_ptr<parser::BinaryOperation> integerLiteral);
        llvm::Value* compile(llvm::raw_ostream &outs, std::shared_ptr<parser::FunctionExpr> functionExpr);
        llvm::Value* compile(llvm::raw_ostream &outs, std::shared_ptr<parser::VarExpr> varExpr);
        llvm::Value* compile(llvm::raw_ostream &outs, std::shared_ptr<parser::BooleanLiteralExpr> booleanLiteralExpr);
        llvm::Value* compile(llvm::raw_ostream &outs, std::shared_ptr<parser::VarAssignmentExpr> varAssignmentExpr);
    
        using Body = std::vector<std::shared_ptr<parser::Stmt>>;
        void compile(llvm::raw_ostream &outs, Body functionStmt);
    
        void declarePrintFunction();
        void declareMallocFunction();
        void declareFreeFunction();
        void declareMemCpyFunction();
        void genAnchorStringStructType();

        llvm::Value* get32BitInteger(int value);
        llvm::Value* getAnchorString(std::string literal);
        llvm::Value* getAnchorStringSize(llvm::Value* anchorString);
        llvm::Value* concat(llvm::Value* lhs, llvm::Value* rhs);
        llvm::Value* malloc(llvm::Value* size);
        void memcpy(llvm::Value* destination, llvm::Value* source, llvm::Value* size);
    public:
        Compiler();
        void compile(llvm::raw_ostream&, parser::Program);
    };

}
#endif 
