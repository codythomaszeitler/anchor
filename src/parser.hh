
#ifndef PARSER_H
#define PARSER_H

#include "lexer.hh"
#include <vector>
#include <deque>
#include <memory>

namespace parser {
    class Stmt {

    };

    class Expr {

    };

    class FunctionStmt : public Stmt {
    public:
        std::string identifier;
        std::vector<std::unique_ptr<Stmt>> stmts;
    };

    class ReturnStmt : public Stmt {
    public:
        ReturnStmt(std::unique_ptr<Expr>);
        std::unique_ptr<Expr> expr;
    };

    class IntegerLiteral : public Expr {
    public:
        int integer;
    };

    enum class Operation {
        ADD
    };

    class BinaryOperation : public Expr {

    public:
        std::unique_ptr<parser::Expr> left;
        parser::Operation operation;
        std::unique_ptr<parser::Expr> right;
    };

    class Parser 
    {
    private:
        std::deque<lexer::Token> tokens;

        std::vector<std::string> args();
        std::string identifier();
        std::vector<std::unique_ptr<Stmt>> block();
        std::unique_ptr<Stmt> stmt();
        std::unique_ptr<Stmt> functionStmt();
        std::unique_ptr<parser::Expr> expr();

        std::unique_ptr<parser::Expr> parseInteger();
        parser::Operation parseOperation();
        std::unique_ptr<parser::Expr> parseBinaryOperation();

        lexer::Token peek();
        lexer::Token pop();
        void consume(lexer::TokenType);
    public:
        Parser(std::deque<lexer::Token>);
        std::vector<std::unique_ptr<Stmt>> parse();
    };
};


#endif // !PARSER_H