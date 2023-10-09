#include "src/compiler.hh"

Compiler::Compiler()
{
    this->context = std::make_unique<llvm::LLVMContext>();
    this->compiling = std::make_unique<llvm::Module>("anchor", *this->context);
    this->builder = std::make_unique<llvm::IRBuilder<>>(*this->context);
}

void Compiler::compile(llvm::raw_ostream &outs, parser::Program program)
{
    for (int i = 0; i < program.stmts.size(); i++)
    {
        std::shared_ptr<parser::Stmt> stmt = program.stmts[i];
        if (stmt->type == parser::StmtType::FUNCTION)
        {
            std::shared_ptr<parser::FunctionStmt> asFunction = program.get<parser::FunctionStmt>(i);
            llvm::FunctionType *voidReturnType = llvm::FunctionType::get(llvm::Type::getVoidTy(*this->context), false);
            llvm::Function *function = llvm::Function::Create(voidReturnType, llvm::Function::ExternalLinkage, asFunction->identifier, *this->compiling);
            llvm::BasicBlock *functionBody = llvm::BasicBlock::Create(*this->context, "body", function);

            for (int j = 0; i < asFunction->stmts.size(); j++) {

            }
        }
    }
}
