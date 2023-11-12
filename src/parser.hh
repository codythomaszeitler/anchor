
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
        VAR_DECL,
        EXPR,
        FUNCTION_ARG,
        IF,
        WHILE,
        BAD
    };

    enum class Type
    {
        VOID,
        INTEGER,
        STRING,
        BOOLEAN,
        NOT_FOUND
    };

    class Stmt
    {
    public:
        parser::StmtType type;
    };

    enum class ExprType
    {
        BINARY_OP,
        INTEGER_LITERAL,
        STRING_LITERAL,
        VAR,
        FUNCTION,
        BOOLEAN,
        ASSIGNMENT
    };

    class Expr
    {
    public:
        parser::ExprType type;
        parser::Type returnType;
    };

    class BooleanLiteralExpr : public Expr
    {
    public:
        bool value;
    };

    class FunctionExpr : public Expr
    {
    public:
        std::string identifier;
        std::vector<std::shared_ptr<parser::Expr>> args;
    };

    class VarExpr : public Expr
    {
    public:
        std::string identifier;
    };

    class FunctionArgStmt : public Stmt
    {
    public:
        std::string identifier;
        parser::Type returnType;
    };

    class FunctionStmt : public Stmt
    {
    public:
        std::string identifier;
        std::vector<std::shared_ptr<parser::FunctionArgStmt>> args;
        std::vector<std::shared_ptr<Stmt>> stmts;
        parser::Type returnType;
    };

    class IfStmt : public Stmt
    {
    public:
        std::shared_ptr<parser::Expr> condition;
        std::vector<std::shared_ptr<Stmt>> stmts;
    };

    class WhileStmt : public Stmt
    {
    public:
        std::shared_ptr<parser::Expr> condition;
        std::vector<std::shared_ptr<Stmt>> stmts;
    };

    class VarDeclStmt : public Stmt
    {
    public:
        std::string identifier;
        parser::Type variableType;
    };

    class VarAssignmentExpr : public Expr
    {
    public:
        std::string identifier;
        std::shared_ptr<parser::Expr> expr;
    };

    class ExprStmt : public Stmt
    {
    public:
        std::shared_ptr<parser::Expr> expr;
    };

    class ReturnStmt : public Stmt
    {
    public:
        explicit ReturnStmt(std::shared_ptr<Expr>);
        std::shared_ptr<Expr> expr;
    };

    class PrintStmt : public Stmt
    {
    public:
        explicit PrintStmt(std::shared_ptr<Expr>);
        std::shared_ptr<Expr> expr;
    };

    class BadStmt : public Stmt
    {
    public:
        lexer::Token offender;
        std::vector<lexer::TokenType> expected;
        std::string message;
        BadStmt(const lexer::Token&, const std::vector<lexer::TokenType>&, const std::string&);
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
        ADD,
        SUBTRACT,
        MULTIPLICATION,
        LESS_THAN,
        GREATER_THAN,
        EQUALS,
        ASSIGNMENT 
    };

    class BinaryOperation : public Expr
    {

    public:
        std::shared_ptr<parser::Expr> left;
        parser::Operation operation;
        std::shared_ptr<parser::Expr> right;
    };

    class InvalidSyntaxException : public std::runtime_error
    {
    private:
        static std::string parseMessage(const lexer::Token& offender, const std::vector<lexer::TokenType>& expected);

    public:
        lexer::Token offender;
        std::vector<lexer::TokenType> expected;

        InvalidSyntaxException(const lexer::Token& offender, const std::vector<lexer::TokenType>& expected);
        const char *what() const throw() override;
    };

    class ErrorLog
    {
    private:
        std::string message;

    public:
        explicit ErrorLog(const std::string&);
        std::string getMessage() const;
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

        bool isSyntacticallyCorrect() const;
        std::vector<parser::ErrorLog> errors;
    };

    class Context {
    private:
        std::map<std::string, parser::Type> varIdToVarType;
        parser::Context* parent = nullptr;
    public:
        void setParent(parser::Context* parent);

        void setType(const std::string&, parser::Type);
        parser::Type getType(const std::string&);
    };

    class Parser
    {
    private:
        parser::Context context;

        std::deque<lexer::Token> tokens;
        parser::Program compiling;

        std::vector<std::shared_ptr<parser::FunctionArgStmt>> args();
        std::string identifier();
        parser::Type type();
        parser::Type parseReturnType();
        std::vector<std::shared_ptr<Stmt>> block();
        std::shared_ptr<Stmt> stmt();
        std::shared_ptr<Stmt> functionStmt();
        std::shared_ptr<Stmt> printStmt();
        std::shared_ptr<Stmt> ifStmt();
        std::shared_ptr<Stmt> whileStmt();
        std::shared_ptr<Stmt> returnStmt();
        std::shared_ptr<Stmt> varDeclStmt();
        std::shared_ptr<Stmt> varAssignmentStmtOrExpr();

        std::shared_ptr<parser::Expr> expr();
        std::shared_ptr<parser::Expr> parseStringLiteral();
        std::shared_ptr<parser::Expr> parseFunctionOrVarExpr();
        std::shared_ptr<parser::Expr> parseInteger();
        std::shared_ptr<parser::Expr> parseBoolean();
        parser::Operation parseOperation();

        lexer::Token peek();
        lexer::Token pop();
        void consume(lexer::TokenType);

    public:
        explicit Parser(const std::deque<lexer::Token>&);
        parser::Program parse();
    };
};

#endif // !PARSER_H