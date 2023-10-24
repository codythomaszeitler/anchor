#include "src/compiler.hh"

namespace compiler
{

    Compiler::Compiler()
    {
        this->context = std::make_unique<llvm::LLVMContext>();
        this->compiling = std::make_unique<llvm::Module>("anchor", *this->context);
        this->builder = std::make_unique<llvm::IRBuilder<>>(*this->context);

        this->declarePrintFunction();
    }

    void Compiler::declarePrintFunction()
    {
        llvm::FunctionType *functionReturnType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*this->context), true);
        llvm::Function::Create(functionReturnType, llvm::Function::ExternalLinkage, "printf", this->compiling.get());
    }

    void Compiler::compile(llvm::raw_ostream &outs, parser::Program program)
    {
        this->compile(outs, program.stmts);
        this->compiling->print(outs, nullptr);
    }

    void Compiler::compile(llvm::raw_ostream &outs, std::shared_ptr<parser::Stmt> stmt)
    {
        if (stmt->type == parser::StmtType::FUNCTION)
        {
            std::shared_ptr<parser::FunctionStmt> functionStmt = std::static_pointer_cast<parser::FunctionStmt>(stmt);
            this->compile(outs, functionStmt);
        }
        else if (stmt->type == parser::StmtType::PRINT)
        {
            std::shared_ptr<parser::PrintStmt> printStmt = std::static_pointer_cast<parser::PrintStmt>(stmt);
            this->compile(outs, printStmt);
        }
        else if (stmt->type == parser::StmtType::RETURN)
        {
            std::shared_ptr<parser::ReturnStmt> returnStmt = std::static_pointer_cast<parser::ReturnStmt>(stmt);
            this->compile(outs, returnStmt);
        }
    }

    void Compiler::compile(llvm::raw_ostream &outs, std::shared_ptr<parser::FunctionStmt> functionStmt)
    {
        llvm::FunctionType *functionReturnType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*this->context), true);
        llvm::Function *function = llvm::Function::Create(functionReturnType, llvm::Function::ExternalLinkage, functionStmt->identifier, this->compiling.get());

        llvm::BasicBlock *bb = llvm::BasicBlock::Create(*this->context, "", function);
        this->builder->SetInsertPoint(bb);

        this->compile(outs, functionStmt->stmts);
    }

    void Compiler::compile(llvm::raw_ostream &outs, Body body)
    {
        for (const auto &stmt : body)
        {
            this->compile(outs, stmt);
        }
    }

    void Compiler::compile(llvm::raw_ostream &outs, std::shared_ptr<parser::PrintStmt> stmt)
    {
        std::vector<llvm::Value *> args;
        if (stmt->expr->returnType == parser::Type::STRING)
        {
            args.push_back(this->builder->CreateGlobalStringPtr(llvm::StringRef("%s")));
        }
        else if (stmt->expr->returnType == parser::Type::INTEGER)
        {
            args.push_back(this->builder->CreateGlobalStringPtr(llvm::StringRef("%d")));
        }

        llvm::Value *expr = this->compile(outs, stmt->expr);
        args.push_back(expr);

        llvm::Function *printf = this->compiling->getFunction("printf");
        this->builder->CreateCall(printf, args);
    }

    llvm::Value *Compiler::compile(llvm::raw_ostream &outs, std::shared_ptr<parser::Expr> expr)
    {
        if (expr->type == parser::ExprType::STRING_LITERAL)
        {
            std::shared_ptr<parser::StringLiteral> stringLiteral = std::static_pointer_cast<parser::StringLiteral>(expr);
            return this->compile(outs, stringLiteral);
        }
        else if (expr->type == parser::ExprType::INTEGER_LITERAL)
        {
            std::shared_ptr<parser::IntegerLiteral> integerLiteral = std::static_pointer_cast<parser::IntegerLiteral>(expr);
            return this->compile(outs, integerLiteral);
        }
        else if (expr->type == parser::ExprType::BINARY_OP) 
        {
            std::shared_ptr<parser::BinaryOperation> binaryOperation = std::static_pointer_cast<parser::BinaryOperation>(expr);
            return this->compile(outs, binaryOperation);
        }
        else
        {
            throw std::invalid_argument("Unsupported expression type.");
        }
    }

    llvm::Value *Compiler::compile(llvm::raw_ostream &outs, std::shared_ptr<parser::StringLiteral> stringLiteral)
    {
        return this->builder->CreateGlobalStringPtr(llvm::StringRef(stringLiteral->literal));
    }

    void Compiler::compile(llvm::raw_ostream &outs, std::shared_ptr<parser::ReturnStmt> returnStmt)
    {
        llvm::Value *expr = this->compile(outs, returnStmt->expr);
        this->builder->CreateRet(expr);
    }

    llvm::Value *Compiler::compile(llvm::raw_ostream &outs, std::shared_ptr<parser::IntegerLiteral> integerLiteral)
    {
        return llvm::ConstantInt::getIntegerValue(llvm::Type::getInt32Ty(*this->context), llvm::APInt(32, integerLiteral->integer));
    }
    
    llvm::Value* Compiler::compile(llvm::raw_ostream &outs, std::shared_ptr<parser::BinaryOperation> integerLiteral)
    {
        llvm::Value* left = this->compile(outs, integerLiteral->left);
        llvm::Value* right = this->compile(outs, integerLiteral->right);
        llvm::Value* result = this->builder->CreateAdd(left, right);
        return result;
    }
}