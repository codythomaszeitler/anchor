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
        this->declareMallocFunction();
        this->declareFreeFunction();
        this->declareMemCpyFunction();
        this->genAnchorStringStructType();
    }

    void Compiler::declarePrintFunction()
    {
        llvm::FunctionType *functionReturnType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*this->context), true);
        llvm::Function::Create(functionReturnType, llvm::Function::ExternalLinkage, "printf", this->compiling.get());
    }

    void Compiler::declareMallocFunction()
    {
        llvm::FunctionType *functionReturnType = llvm::FunctionType::get(llvm::Type::getInt32PtrTy(*this->context), true);
        llvm::Function::Create(functionReturnType, llvm::Function::ExternalLinkage, "malloc", this->compiling.get());
    }

    void Compiler::declareFreeFunction()
    {
        llvm::FunctionType *functionReturnType = llvm::FunctionType::get(llvm::Type::getVoidTy(*this->context), true);
        llvm::Function::Create(functionReturnType, llvm::Function::ExternalLinkage, "free", this->compiling.get());
    }

    void Compiler::declareMemCpyFunction()
    {
        llvm::FunctionType *functionReturnType = llvm::FunctionType::get(llvm::Type::getVoidTy(*this->context), true);
        llvm::Function::Create(functionReturnType, llvm::Function::ExternalLinkage, "memcpy", this->compiling.get());
    }

    void Compiler::genAnchorStringStructType()
    {
        llvm::Type *characterBufferPointer = llvm::Type::getInt8PtrTy(*this->context);
        llvm::Type *numCharsCurrentlyInBuffer = llvm::Type::getInt32Ty(*this->context);
        this->anchorStringStructType = llvm::StructType::create(std::vector<llvm::Type *>{characterBufferPointer, numCharsCurrentlyInBuffer});
    }

    llvm::Value *Compiler::get32BitInteger(int value)
    {
        return llvm::ConstantInt::getIntegerValue(llvm::Type::getInt32Ty(*this->context), llvm::APInt(32, value));
    }

    llvm::Value *Compiler::getAnchorString(const std::string &literal)
    {
        llvm::Value *source = this->builder->CreateGlobalStringPtr(llvm::StringRef(literal), "", 0U, this->compiling.get());

        llvm::StructType *structType = this->anchorStringStructType;
        llvm::Value *anchorString = this->builder->CreateAlloca(structType);

        llvm::Value *size = this->get32BitInteger(static_cast<int>(literal.length() + 1));
        llvm::Value *allocated = this->malloc(size);

        llvm::Value *stringBuffer = this->builder->CreateStructGEP(structType, anchorString, 0);

        this->memcpy(allocated, source, size);
        this->builder->CreateStore(allocated, stringBuffer);

        llvm::Value *sizePointer = this->builder->CreateStructGEP(structType, anchorString, 1);
        this->builder->CreateStore(size, sizePointer);
        return anchorString;
    }

    llvm::Value *Compiler::malloc(llvm::Value *size)
    {
        llvm::Function *malloc = this->compiling->getFunction("malloc");
        llvm::Value *allocated = this->builder->CreateCall(malloc, std::vector<llvm::Value *>{size});
        return allocated;
    }

    void Compiler::memcpy(llvm::Value *destination, llvm::Value *source, llvm::Value *size)
    {
        llvm::Function *memcpy = this->compiling->getFunction("memcpy");
        this->builder->CreateCall(memcpy, std::vector<llvm::Value *>{destination, source, size});
    }

    void Compiler::compile(llvm::raw_ostream &outs, const parser::Program &program)
    {
        this->compile(program.stmts);
        this->compiling->print(outs, nullptr);
    }

    void Compiler::compile(std::shared_ptr<parser::Stmt> stmt)
    {
        using enum parser::StmtType;
        if (stmt->type == FUNCTION)
        {
            std::shared_ptr<parser::FunctionStmt> functionStmt = std::static_pointer_cast<parser::FunctionStmt>(stmt);
            this->compile(functionStmt);
        }
        else if (stmt->type == PRINT)
        {
            std::shared_ptr<parser::PrintStmt> printStmt = std::static_pointer_cast<parser::PrintStmt>(stmt);
            this->compile(printStmt);
        }
        else if (stmt->type == RETURN)
        {
            std::shared_ptr<parser::ReturnStmt> returnStmt = std::static_pointer_cast<parser::ReturnStmt>(stmt);
            this->compile(returnStmt);
        }
        else if (stmt->type == VAR_DECL)
        {
            std::shared_ptr<parser::VarDeclStmt> varDeclStmt = std::static_pointer_cast<parser::VarDeclStmt>(stmt);
            this->compile(varDeclStmt);
        }
        else if (stmt->type == EXPR)
        {
            std::shared_ptr<parser::ExprStmt> exprStmt = std::static_pointer_cast<parser::ExprStmt>(stmt);
            this->compile(exprStmt);
        }
        else if (stmt->type == IF)
        {
            std::shared_ptr<parser::IfStmt> ifStmt = std::static_pointer_cast<parser::IfStmt>(stmt);
            this->compile(ifStmt);
        }
        else if (stmt->type == WHILE)
        {
            std::shared_ptr<parser::WhileStmt> whileStmt = std::static_pointer_cast<parser::WhileStmt>(stmt);
            this->compile(whileStmt);
        }
    }

    void Compiler::compile(std::shared_ptr<parser::FunctionStmt> functionStmt)
    {
        llvm::Function *function = this->getFunctionWithNamedParams(functionStmt);

        llvm::BasicBlock *prev = this->builder->GetInsertBlock();

        llvm::BasicBlock *functionBlock = llvm::BasicBlock::Create(*this->context, "", function);
        this->builder->SetInsertPoint(functionBlock);

        this->compile(functionStmt->stmts);

        if (functionStmt->returnType == parser::Type::VOID)
        {
            this->builder->CreateRetVoid();
        }

        this->builder->SetInsertPoint(prev);
    }

    llvm::Function *Compiler::getFunctionWithNamedParams(std::shared_ptr<parser::FunctionStmt> astFunctionStmt)
    {
        llvm::FunctionType *functionType = this->functionType(astFunctionStmt);
        llvm::Function *llvmFunction = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, astFunctionStmt->identifier, this->compiling.get());
        for (int i = 0; i < astFunctionStmt->args.size(); i++)
        {
            const auto astArg = astFunctionStmt->args[i];
            const auto llvmArg = llvmFunction->getArg(i);
            llvmArg->setName(astArg->identifier);
        }
        return llvmFunction;
    }

    llvm::FunctionType *Compiler::functionType(std::shared_ptr<parser::FunctionStmt> functionStmt)
    {
        std::vector<llvm::Type *> args = this->functionStmtArgTypes(functionStmt->args);

        llvm::FunctionType *functionReturnType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*this->context), args, true);
        if (functionStmt->returnType == parser::Type::STRING)
        {
            llvm::Type *returnType = llvm::FunctionType::getInt32PtrTy(*this->context);
            functionReturnType = llvm::FunctionType::get(returnType, args, false);
        }
        else if (functionStmt->returnType == parser::Type::VOID)
        {
            llvm::Type *returnType = llvm::FunctionType::getVoidTy(*this->context);
            functionReturnType = llvm::FunctionType::get(returnType, args, false);
        }

        return functionReturnType;
    }

    std::vector<llvm::Type *> Compiler::functionStmtArgTypes(const std::vector<std::shared_ptr<parser::FunctionArgStmt>> &args)
    {
        std::vector<llvm::Type *> compiledArgs;
        for (const auto &arg : args)
        {
            compiledArgs.push_back(llvm::Type::getInt64PtrTy(*this->context));
        }
        return compiledArgs;
    }

    void Compiler::compile(const Body &body)
    {
        for (const auto &stmt : body)
        {
            this->compile(stmt);
        }
    }

    void Compiler::compile(std::shared_ptr<parser::PrintStmt> stmt)
    {
        std::vector<llvm::Value *> args;
        if (stmt->expr->returnType == parser::Type::STRING)
        {
            args.push_back(this->builder->CreateGlobalStringPtr(llvm::StringRef("%s"), "", 0U, this->compiling.get()));
        }
        else if (stmt->expr->returnType == parser::Type::INTEGER || stmt->expr->returnType == parser::Type::BOOLEAN)
        {
            args.push_back(this->builder->CreateGlobalStringPtr(llvm::StringRef("%d")));
        }

        llvm::Value *expr = this->compile(stmt->expr);

        if (stmt->expr->returnType == parser::Type::STRING)
        {
            llvm::Value *stringLiteralPointer = this->builder->CreateStructGEP(this->anchorStringStructType, expr, 0);
            llvm::Value *stringLiteral = this->builder->CreateLoad(llvm::Type::getInt8PtrTy(*this->context), stringLiteralPointer);

            args.push_back(stringLiteral);
            llvm::Function *printf = this->compiling->getFunction("printf");
            this->builder->CreateCall(printf, args);
        }
        else
        {
            args.push_back(expr);
            llvm::Function *printf = this->compiling->getFunction("printf");
            this->builder->CreateCall(printf, args);
        }
    }

    llvm::Value *Compiler::compile(std::shared_ptr<parser::Expr> expr)
    {
        using enum parser::ExprType;
        if (expr->type == STRING_LITERAL)
        {
            std::shared_ptr<parser::StringLiteral> stringLiteral = std::static_pointer_cast<parser::StringLiteral>(expr);
            return this->compile(stringLiteral);
        }
        else if (expr->type == INTEGER_LITERAL)
        {
            std::shared_ptr<parser::IntegerLiteral> integerLiteral = std::static_pointer_cast<parser::IntegerLiteral>(expr);
            return this->compile(integerLiteral);
        }
        else if (expr->type == BINARY_OP)
        {
            std::shared_ptr<parser::BinaryOperation> binaryOperation = std::static_pointer_cast<parser::BinaryOperation>(expr);
            return this->compile(binaryOperation);
        }
        else if (expr->type == FUNCTION)
        {
            std::shared_ptr<parser::FunctionExpr> functionExpr = std::static_pointer_cast<parser::FunctionExpr>(expr);
            return this->compile(functionExpr);
        }
        else if (expr->type == VAR)
        {
            std::shared_ptr<parser::VarExpr> varExpr = std::static_pointer_cast<parser::VarExpr>(expr);
            return this->compile(varExpr);
        }
        else if (expr->type == BOOLEAN)
        {
            std::shared_ptr<parser::BooleanLiteralExpr> varExpr = std::static_pointer_cast<parser::BooleanLiteralExpr>(expr);
            return this->compile(varExpr);
        }
        else if (expr->type == ASSIGNMENT)
        {
            std::shared_ptr<parser::VarAssignmentExpr> varExpr = std::static_pointer_cast<parser::VarAssignmentExpr>(expr);
            return this->compile(varExpr);
        }
        else
        {
            throw std::invalid_argument("Unsupported expression type.");
        }
    }

    llvm::Value *Compiler::compile(const std::shared_ptr<parser::StringLiteral> stringLiteral)
    {
        return this->getAnchorString(stringLiteral->literal);
    }

    void Compiler::compile(std::shared_ptr<parser::ReturnStmt> returnStmt)
    {
        llvm::Value *expr = this->compile(returnStmt->expr);
        this->builder->CreateRet(expr);
    }

    llvm::Value *Compiler::compile(std::shared_ptr<parser::IntegerLiteral> integerLiteral)
    {
        return llvm::ConstantInt::getIntegerValue(llvm::Type::getInt32Ty(*this->context), llvm::APInt(32, integerLiteral->integer));
    }

    llvm::Value *Compiler::compile(std::shared_ptr<parser::BinaryOperation> binaryOp)
    {
        llvm::Value *left = this->compile(binaryOp->left);
        llvm::Value *right = this->compile(binaryOp->right);

        if (binaryOp->returnType == parser::Type::STRING)
        {
            return this->concat(left, right);
        }

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
        else if (binaryOp->operation == parser::Operation::LESS_THAN)
        {
            return this->builder->CreateICmpSLT(left, right);
        }
        else if (binaryOp->operation == parser::Operation::GREATER_THAN)
        {
            return this->builder->CreateICmpSGT(left, right);
        }
        else if (binaryOp->operation == parser::Operation::EQUALS)
        {
            return this->builder->CreateICmpEQ(left, right);
        }
        else
        {
            throw std::invalid_argument("Unsupported parser::Operation");
        }
    }

    llvm::Value *Compiler::compile(std::shared_ptr<parser::FunctionExpr> functionExpr)
    {
        llvm::Function *function = this->compiling->getFunction(llvm::StringRef(functionExpr->identifier));

        std::vector<llvm::Value *> args;
        for (const auto &arg : functionExpr->args)
        {
            llvm::Value *value = this->compile(arg);
            llvm::Value *stackAllocation = this->builder->CreateAlloca(llvm::Type::getInt32Ty(*this->context));
            this->builder->CreateStore(value, stackAllocation);
            args.push_back(stackAllocation);
        }

        return this->builder->CreateCall(function, args);
    }

    void Compiler::compile(std::shared_ptr<parser::VarDeclStmt> varDeclStmt)
    {
        if (varDeclStmt->variableType == parser::Type::INTEGER)
        {
            llvm::Value *value = this->builder->CreateAlloca(llvm::Type::getInt32Ty(*this->context), nullptr, varDeclStmt->identifier);
            llvm::Value *zero = llvm::ConstantInt::getIntegerValue(llvm::Type::getInt32Ty(*this->context), llvm::APInt(32, 0));
            this->builder->CreateStore(zero, value);
        }
        else if (varDeclStmt->variableType == parser::Type::STRING)
        {
            llvm::Value *emptyString = this->getAnchorString("");
            llvm::Value *value = this->builder->CreateAlloca(llvm::Type::getInt32PtrTy(*this->context), nullptr, varDeclStmt->identifier);
            this->builder->CreateStore(emptyString, value);
        }
        else if (varDeclStmt->variableType == parser::Type::BOOLEAN)
        {
            llvm::Value *value = this->builder->CreateAlloca(llvm::Type::getInt1Ty(*this->context), nullptr, varDeclStmt->identifier);
            llvm::Value *zero = llvm::ConstantInt::getBool(llvm::Type::getInt1Ty(*this->context), false);
            this->builder->CreateStore(zero, value);
        }
    }

    void Compiler::compile(std::shared_ptr<parser::ExprStmt> exprStmt)
    {
        this->compile(exprStmt->expr);
    }

    llvm::Value *Compiler::compile(std::shared_ptr<parser::VarExpr> varExpr)
    {
        llvm::BasicBlock *bb = this->builder->GetInsertBlock();
        llvm::Value *value = bb->getValueSymbolTable()->lookup(llvm::StringRef(varExpr->identifier));

        if (varExpr->returnType == parser::Type::BOOLEAN)
        {
            return this->builder->CreateLoad(llvm::Type::getInt1Ty(*this->context), value);
        }
        else if (varExpr->returnType == parser::Type::INTEGER)
        {
            return this->builder->CreateLoad(llvm::Type::getInt32Ty(*this->context), value);
        }
        else if (varExpr->returnType == parser::Type::STRING)
        {
            return this->builder->CreateLoad(llvm::Type::getInt8PtrTy(*this->context), value);
        }
        else if (varExpr->returnType == parser::Type::VOID)
        {
            return this->builder->CreateLoad(llvm::Type::getVoidTy(*this->context), value);
        }
        else
        {
            throw std::invalid_argument("Cannot compile variable expression with unknown type.");
        }
    }

    llvm::Value *Compiler::concat(llvm::Value *lhs, llvm::Value *rhs)
    {
        llvm::Value *lhsSize = this->getAnchorStringSize(lhs);
        llvm::Value *rhsSize = this->getAnchorStringSize(rhs);

        llvm::Value *concatSizeWithNullTerminators = this->builder->CreateAdd(lhsSize, rhsSize);
        llvm::Value *one = this->get32BitInteger(1);

        llvm::Value *concatSize = this->builder->CreateSub(concatSizeWithNullTerminators, one);
        llvm::Value *allocated = this->malloc(concatSize);

        llvm::Value *anchorString = this->builder->CreateAlloca(this->anchorStringStructType);
        llvm::Value *anchorStringCharBufferPointer = this->builder->CreateStructGEP(this->anchorStringStructType, anchorString, 0);
        llvm::Value *anchorStringCharBuffer = this->builder->CreateLoad(llvm::Type::getInt8PtrTy(*this->context), anchorStringCharBufferPointer);

        llvm::Value *lhsCharBufferPointer = this->builder->CreateStructGEP(this->anchorStringStructType, lhs, 0);
        llvm::Value *lhsCharBuffer = this->builder->CreateLoad(llvm::Type::getInt8PtrTy(*this->context), lhsCharBufferPointer);

        llvm::Value *rhsCharBufferPointer = this->builder->CreateStructGEP(this->anchorStringStructType, rhs, 0);
        llvm::Value *rhsCharBuffer = this->builder->CreateLoad(llvm::Type::getInt8PtrTy(*this->context), rhsCharBufferPointer);

        llvm::Value *endOfLhs = this->builder->CreateInBoundsGEP(llvm::Type::getInt8Ty(*this->context), allocated, std::vector<llvm::Value *>{this->builder->CreateSub(lhsSize, one)});
        this->memcpy(allocated, lhsCharBuffer, this->builder->CreateSub(lhsSize, one));
        this->memcpy(endOfLhs, rhsCharBuffer, rhsSize);

        this->builder->CreateStore(allocated, anchorStringCharBufferPointer);

        llvm::Value *anchorStringCharLength = this->builder->CreateStructGEP(this->anchorStringStructType, anchorString, 1);
        this->builder->CreateStore(concatSize, anchorStringCharLength);
        return anchorString;
    }

    llvm::Value *Compiler::getAnchorStringSize(llvm::Value *anchorString)
    {
        llvm::Value *pointerToSize = this->builder->CreateStructGEP(this->anchorStringStructType, anchorString, 1);
        llvm::Value *size = this->builder->CreateLoad(llvm::Type::getInt32Ty(*this->context), pointerToSize);
        return size;
    }

    llvm::Value *Compiler::compile(std::shared_ptr<parser::BooleanLiteralExpr> booleanLiteralExpr)
    {
        llvm::Value *value = llvm::ConstantInt::getBool(llvm::Type::getInt1Ty(*this->context), booleanLiteralExpr->value);
        return value;
    }

    void Compiler::compile(std::shared_ptr<parser::IfStmt> ifStmt)
    {
        llvm::BasicBlock *prev = this->builder->GetInsertBlock();
        llvm::BasicBlock *then = llvm::BasicBlock::Create(*this->context, "then", prev->getParent());
        llvm::BasicBlock *end = llvm::BasicBlock::Create(*this->context, "end", prev->getParent());

        llvm::Value *condition = this->compile(ifStmt->condition);
        this->builder->CreateCondBr(condition, then, end);

        this->builder->SetInsertPoint(then);
        this->compile(ifStmt->stmts);
        this->builder->CreateBr(end);

        this->builder->SetInsertPoint(end);
    }

    void Compiler::compile(std::shared_ptr<parser::WhileStmt> whileStmt)
    {
        llvm::BasicBlock *prev = this->builder->GetInsertBlock();

        llvm::BasicBlock *whileLoopStart = llvm::BasicBlock::Create(*this->context, "whileLoopStart", prev->getParent());
        llvm::BasicBlock *body = llvm::BasicBlock::Create(*this->context, "body", prev->getParent());
        llvm::BasicBlock *end = llvm::BasicBlock::Create(*this->context, "end", prev->getParent());

        this->builder->CreateBr(whileLoopStart);

        this->builder->SetInsertPoint(whileLoopStart);
        llvm::Value *condition = this->compile(whileStmt->condition);
        this->builder->CreateCondBr(condition, body, end);

        this->builder->SetInsertPoint(body);
        this->compile(whileStmt->stmts);
        this->builder->CreateBr(whileLoopStart);

        this->builder->SetInsertPoint(end);
    }

    llvm::Value *Compiler::compile(std::shared_ptr<parser::VarAssignmentExpr> varAssignmentExpr)
    {
        llvm::BasicBlock *bb = this->builder->GetInsertBlock();
        llvm::Value *value = bb->getValueSymbolTable()->lookup(llvm::StringRef(varAssignmentExpr->identifier));
        llvm::Value *rhs = this->compile(varAssignmentExpr->expr);
        return this->builder->CreateStore(rhs, value);
    }
}