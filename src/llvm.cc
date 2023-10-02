#include "src/llvm.hh"
#include <string>
#include <memory>
#include <algorithm>
#include <vector>

namespace llvm
{
    Parser::Parser(parser::Program program) : program(program)
    {
        
    }

    llvm::Program Parser::parse()
    {
        llvm::Program program;

        std::for_each(this->program.stmts.begin(), this->program.stmts.end(), [&](std::shared_ptr<parser::Stmt> stmt) {
            if (stmt->type == parser::StmtType::FUNCTION) {
                std::shared_ptr<parser::FunctionStmt> anchorFunction = std::static_pointer_cast<parser::FunctionStmt>(stmt);
                std::shared_ptr<llvm::Function> llvmFunction = std::shared_ptr<llvm::Function>(new llvm::Function());

                llvmFunction->identifier = anchorFunction->identifier;

                std::for_each(anchorFunction->stmts.begin(), anchorFunction->stmts.end(), [&](std::shared_ptr<parser::Stmt> stmt) {
                    if (stmt->type == parser::StmtType::RETURN) {
                        std::shared_ptr<parser::ReturnStmt> anchorReturnStmt = std::static_pointer_cast<parser::ReturnStmt>(stmt);

                        std::vector<std::shared_ptr<llvm::Instruction>> instructions = this->expr(anchorReturnStmt->expr);

                        std::shared_ptr<llvm::RetInstruction> retInstruction(new llvm::RetInstruction());
                        retInstruction->irType = llvm::InstructionType::RET;
                        retInstruction->type = llvm::Type::I32;
                        retInstruction->inputIdentifier = "0";
                        instructions.push_back(retInstruction);

                        llvmFunction->instructions = instructions;
                    }
                });

                program.functionByName[llvmFunction->identifier] = llvmFunction;
            }
        });

        return program;
    }

    std::vector<std::shared_ptr<llvm::Instruction>> Parser::expr(std::shared_ptr<parser::Expr> expr) 
    {
        std::vector<std::shared_ptr<llvm::Instruction>> instructions;
        if (expr->type == parser::ExprType::BINARY_OP) {
            std::shared_ptr<parser::BinaryOperation> binaryOp = std::static_pointer_cast<parser::BinaryOperation>(expr);
            std::shared_ptr<parser::IntegerLiteral> left = std::static_pointer_cast<parser::IntegerLiteral>(binaryOp->left); 
            std::shared_ptr<parser::IntegerLiteral> right = std::static_pointer_cast<parser::IntegerLiteral>(binaryOp->right);

            std::shared_ptr<llvm::AddInstruction> addInstruction(new llvm::AddInstruction());

            addInstruction->irType = llvm::InstructionType::ADD;
            addInstruction->left = left->integer;
            addInstruction->right = right->integer;
            addInstruction->type = llvm::Type::I32;
            addInstruction->outputIdentifier = "0";

            instructions.push_back(addInstruction);
        } 
        return instructions;
    }

    std::shared_ptr<llvm::Function> Program::getFunction(std::string identifier)
    {
        return this->functionByName[identifier];
    }
}