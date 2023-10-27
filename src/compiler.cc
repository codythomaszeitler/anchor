#include "src/compiler.hh"
#include "llvm/IR/ValueSymbolTable.h"

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
        else if (stmt->type == parser::StmtType::VAR_DECL)
        {
            std::shared_ptr<parser::VarDeclStmt> varDeclStmt = std::static_pointer_cast<parser::VarDeclStmt>(stmt);
            this->compile(outs, varDeclStmt);
        }
        else if (stmt->type == parser::StmtType::VAR_ASSIGNMENT)
        {
            std::shared_ptr<parser::VarAssignmentStmt> varAssignmentStmt = std::static_pointer_cast<parser::VarAssignmentStmt>(stmt);
            this->compile(outs, varAssignmentStmt);
        }
    }

    void Compiler::compile(llvm::raw_ostream &outs, std::shared_ptr<parser::FunctionStmt> functionStmt)
    {
        std::vector<llvm::Type *> args;
        for (const auto &arg : functionStmt->args)
        {
            args.push_back(llvm::Type::getInt64PtrTy(*this->context));
        }

        llvm::FunctionType *functionReturnType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*this->context), args, true);
        llvm::Function *function = llvm::Function::Create(functionReturnType, llvm::Function::ExternalLinkage, functionStmt->identifier, this->compiling.get());

        for (int i = 0; i < functionStmt->args.size(); i++) 
        {
            const auto astArg = functionStmt->args[i];
            const auto llvmArg = function->getArg(i);
            llvmArg->setName(astArg->identifier);
        }

        llvm::BasicBlock *prev = this->builder->GetInsertBlock();

        llvm::BasicBlock *bb = llvm::BasicBlock::Create(*this->context, "", function);
        this->builder->SetInsertPoint(bb);

        this->compile(outs, functionStmt->stmts);

        this->builder->SetInsertPoint(prev);
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
        else if (expr->type == parser::ExprType::FUNCTION)
        {
            std::shared_ptr<parser::FunctionExpr> functionExpr = std::static_pointer_cast<parser::FunctionExpr>(expr);
            return this->compile(outs, functionExpr);
        }
        else if (expr->type == parser::ExprType::VAR)
        {
            std::shared_ptr<parser::VarExpr> varExpr = std::static_pointer_cast<parser::VarExpr>(expr);
            return this->compile(outs, varExpr);
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

    llvm::Value *Compiler::compile(llvm::raw_ostream &outs, std::shared_ptr<parser::BinaryOperation> binaryOp)
    {
        llvm::Value *left = this->compile(outs, binaryOp->left);
        llvm::Value *right = this->compile(outs, binaryOp->right);

        if (binaryOp->operation == parser::Operation::ADD)
        {
            return this->builder->CreateAdd(left, right);
        }
        else if (binaryOp->operation == parser::Operation::SUBTRACT)
        {
            return this->builder->CreateSub(left, right);
        }
        else if (binaryOp->operation == parser::Operation::MULTIPLICATION)
        {
            return this->builder->CreateMul(left, right);
        }
        else
        {
            throw std::invalid_argument("Unsupported parser::Operation");
        }
    }

    llvm::Value *Compiler::compile(llvm::raw_ostream &outs, std::shared_ptr<parser::FunctionExpr> functionExpr)
    {
        llvm::Function *function = this->compiling->getFunction(llvm::StringRef(functionExpr->identifier));

        std::vector<llvm::Value *> args;
        for (const auto &arg : functionExpr->args)
        {
            llvm::Value *value = this->compile(outs, arg);
            llvm::Value* stackAllocation = this->builder->CreateAlloca(llvm::Type::getInt32Ty(*this->context));
            this->builder->CreateStore(value, stackAllocation);
            args.push_back(stackAllocation);
        }

        return this->builder->CreateCall(function, args);
    }

    void Compiler::compile(llvm::raw_ostream &outs, std::shared_ptr<parser::VarDeclStmt> varDeclStmt)
    {
        llvm::Value *value = this->builder->CreateAlloca(llvm::Type::getInt32Ty(*this->context), nullptr, varDeclStmt->identifier);
        llvm::Value *zero = llvm::ConstantInt::getIntegerValue(llvm::Type::getInt32Ty(*this->context), llvm::APInt(32, 0));
        this->builder->CreateStore(zero, value);
    }

    void Compiler::compile(llvm::raw_ostream &outs, std::shared_ptr<parser::VarAssignmentStmt> varAssignmentStmt)
    {
        llvm::BasicBlock *bb = this->builder->GetInsertBlock();
        llvm::Value *value = bb->getValueSymbolTable()->lookup(llvm::StringRef(varAssignmentStmt->identifier));
        llvm::Value *rhs = this->compile(outs, varAssignmentStmt->expr);
        this->builder->CreateStore(rhs, value);
    }

    llvm::Value *Compiler::compile(llvm::raw_ostream &outs, std::shared_ptr<parser::VarExpr> varExpr)
    {
        llvm::BasicBlock *bb = this->builder->GetInsertBlock();
        llvm::Value *value = bb->getValueSymbolTable()->lookup(llvm::StringRef(varExpr->identifier));
        return this->builder->CreateLoad(llvm::Type::getInt32Ty(*this->context), value);
    }
}