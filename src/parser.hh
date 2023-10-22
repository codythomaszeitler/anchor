
#ifndef PARSER_H
#define PARSER_H

#include "lexer.hh"
#include <vector>
#include <deque>
#include <memory>

namespace parser
{
    enum class StmtType
    {
        FUNCTION,
        RETURN,
        PRINT,
        BAD
    };

    class Stmt
    {
    public:
        parser::StmtType type;
    };

    enum class ExprType {
        BINARY_OP,
        INTEGER_LITERAL,
        STRING_LITERAL
    };

    class Expr
    {
    public:
        parser::ExprType type;
    };

    class FunctionStmt : public Stmt
    {
    public:
        std::string identifier;
        std::vector<std::shared_ptr<Stmt>> stmts;
    };

    class ReturnStmt : public Stmt
    {
    public:
        ReturnStmt(std::shared_ptr<Expr>);
        std::shared_ptr<Expr> expr;
    };

    class PrintStmt : public Stmt 
    {
    public:
        PrintStmt(std::shared_ptr<Expr>);
        std::shared_ptr<Expr> expr;
    };

    class IntegerLiteral : public Expr
    {
    public:
        int integer;
    };

    class StringLiteral : public Expr 
    {
    public:
        std::string literal;
    };

    enum class Operation
    {
        ADD
    };

    class BinaryOperation : public Expr
    {

    public:
        std::shared_ptr<parser::Expr> left;
        parser::Operation operation;
        std::shared_ptr<parser::Expr> right;
    };

    class InvalidSyntaxException : public std::runtime_error {
    private:
        lexer::Token offender;
        std::vector<lexer::TokenType> expected;
        static std::string parseMessage(lexer::Token offender, std::vector<lexer::TokenType> expected);
    public:
        InvalidSyntaxException(lexer::Token offender, std::vector<lexer::TokenType> expected);        
        virtual const char* what() const throw();
    };

    class ErrorLog 
    {
    private:
        lexer::Token offender;
        std::vector<lexer::TokenType> expected;
    public:
        ErrorLog(lexer::Token offender, std::vector<lexer::TokenType> expected); 
        std::string message();
    };

    class Program : public Stmt
    {
    public:
        std::vector<std::shared_ptr<Stmt>> stmts;
        template <typename T>
        std::shared_ptr<T> get(int index)
        {
            return std::static_pointer_cast<T>(this->stmts.at(index));
        }

        bool isSyntacticallyCorrect();
        std::vector<parser::ErrorLog> errors();
    };

    class Parser
    {
    private:
        std::deque<lexer::Token> tokens;

        std::vector<std::string> args();
        std::string identifier();
        std::vector<std::shared_ptr<Stmt>> block();
        std::shared_ptr<Stmt> stmt();
        std::shared_ptr<Stmt> functionStmt();
        std::shared_ptr<Stmt> printStmt();
        std::shared_ptr<Stmt> returnStmt();
        std::shared_ptr<parser::Expr> expr();

        std::shared_ptr<parser::Expr> parseStringLiteral();
        std::shared_ptr<parser::Expr> parseInteger();
        parser::Operation parseOperation();
        std::shared_ptr<parser::Expr> parseBinaryOperation();

        lexer::Token peek();
        lexer::Token pop();
        void consume(lexer::TokenType);

    public:
        Parser(std::deque<lexer::Token>);
        parser::Program parse();
    };
};

#endif // !PARSER_H