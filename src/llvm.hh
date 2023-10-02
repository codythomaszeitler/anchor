#ifndef __LLVM_H__
#define __LLVM_H__

#include "parser.hh"
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

namespace llvm
{

    class Program;
    class Function;
    class Instruction;
    enum class Type;
    enum class InstructionType;
    class AddInstruction;
    class RetInstruction;

    class Parser
    {
    private:
        parser::Program program;
        std::vector<std::shared_ptr<llvm::Instruction>> expr(std::shared_ptr<parser::Expr>);

    public:
        Parser(parser::Program);
        llvm::Program parse();
    };

    class Program
    {
    public:
        std::unordered_map<std::string, std::shared_ptr<llvm::Function>> functionByName;
        std::shared_ptr<llvm::Function> getFunction(std::string);
    };

    class Function
    {
    public:
        std::string identifier;
        llvm::Type returnType;
        std::vector<std::shared_ptr<llvm::Instruction>> instructions;

        template <typename T>
        std::shared_ptr<T> getInstruction(int index)
        {
            return std::static_pointer_cast<T>(this->instructions.at(index));
        }
    };

    class Instruction
    {
    public:
        llvm::InstructionType irType;
    };

    class AddInstruction : public Instruction
    {
    public:
        int left;
        int right;
        std::string outputIdentifier;
        llvm::Type type;
    };

    class RetInstruction : public Instruction
    {
    public:
        std::string inputIdentifier;
        llvm::Type type;
    };

    enum class InstructionType
    {
        ADD,
        RET
    };

    enum class Type
    {
        I32
    };


    class Writer {
    private:
        std::string write(llvm::Type);
    
    public:
        std::string write(llvm::Program);
        std::string write(std::shared_ptr<llvm::AddInstruction>);
        std::string write(std::shared_ptr<llvm::RetInstruction>);
    };
}

#endif // __LLVM_H__