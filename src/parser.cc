#include "parser.hh"

#include <vector>
#include <deque>
#include "lexer.hh"
#include <iostream>
#include <memory>
#include <iostream>
#include <string>
#include <algorithm>

namespace parser
{
    ReturnStmt::ReturnStmt(std::shared_ptr<Expr> expr) : expr(std::move(expr))
    {
    }

    Parser::Parser(std::deque<lexer::Token> tokens) : tokens(tokens)
    {
    }

    parser::Program Parser::parse()
    {
        std::vector<std::shared_ptr<Stmt>> stmts;
        while (this->tokens.size() != 0)
        {
            stmts.push_back(this->stmt());
        }

        parser::Program program;
        program.stmts = stmts;
        return program;
    };

    std::shared_ptr<Stmt> Parser::stmt()
    {
        lexer::Token maybeReturnOrFunctionDefinition = this->peek();

        if (maybeReturnOrFunctionDefinition.getTokenType() == lexer::TokenType::RETURN)
        {
            this->consume(lexer::TokenType::RETURN);

            parser::ReturnStmt *returnStmtPointer = new parser::ReturnStmt(this->expr());
            returnStmtPointer->type = parser::StmtType::RETURN;
            std::shared_ptr<parser::Stmt> returnStmt(returnStmtPointer);
            this->consume(lexer::TokenType::SEMICOLON);
            return returnStmt;
        }
        else
        {
            return this->functionStmt();
        }
    }

    std::shared_ptr<Stmt> Parser::functionStmt()
    {
        this->consume(lexer::TokenType::FUNCTION);
        std::string identifier = this->identifier();

        this->args();

        parser::FunctionStmt *functionStmt = new parser::FunctionStmt();
        functionStmt->type = parser::StmtType::FUNCTION;
        functionStmt->stmts = this->block();
        functionStmt->identifier = identifier;
        this->consume(lexer::TokenType::SEMICOLON);
        return std::shared_ptr<Stmt>(functionStmt);
    }

    std::string Parser::identifier()
    {
        lexer::Token token = this->pop();
        return token.getRaw();
    }

    std::vector<std::string> Parser::args()
    {
        this->consume(lexer::TokenType::LEFT_PAREN);
        this->consume(lexer::TokenType::RIGHT_PAREN);
        return std::vector<std::string>();
    }

    std::vector<std::shared_ptr<Stmt>> Parser::block()
    {
        this->consume(lexer::TokenType::LEFT_BRACKET);
        std::vector<std::shared_ptr<Stmt>> stmts;

        while (this->peek().getTokenType() != lexer::TokenType::RIGHT_BRACKET)
        {
            stmts.push_back(this->stmt());
        }

        this->consume(lexer::TokenType::RIGHT_BRACKET);
        return stmts;
    }

    void Parser::consume(lexer::TokenType tokenType)
    {
        lexer::Token consumed = this->pop();
        if (consumed.getTokenType() != tokenType)
        {
            std::cout << "Someone somewhere did not have the correct token..." << std::endl;
        }
    }

    lexer::Token Parser::pop()
    {
        lexer::Token token = this->peek();
        this->tokens.pop_front();
        return token;
    }

    lexer::Token Parser::peek()
    {
        return this->tokens.front();
    }

    std::shared_ptr<parser::Expr> Parser::expr()
    {
        return this->parseBinaryOperation();
    }

    std::shared_ptr<parser::Expr> Parser::parseBinaryOperation()
    {
        parser::BinaryOperation *binaryOperation = new parser::BinaryOperation();
        binaryOperation->left = this->parseInteger();
        binaryOperation->operation = this->parseOperation();
        binaryOperation->right = this->parseInteger();
        binaryOperation->type = parser::ExprType::BINARY_OP;
        return std::shared_ptr<parser::Expr>(binaryOperation);
    }

    std::shared_ptr<parser::Expr> Parser::parseInteger()
    {
        lexer::Token integerToken = this->pop();
        std::string intAsString = integerToken.getRaw();

        parser::IntegerLiteral *integerLiteral = new parser::IntegerLiteral();
        integerLiteral->integer = stoi(intAsString);
        integerLiteral->type = parser::ExprType::INTEGER_LITERAL;
        return std::shared_ptr<parser::Expr>(integerLiteral);
    }

    parser::Operation Parser::parseOperation()
    {
        lexer::Token plus = this->pop();
        return parser::Operation::ADD;
    }
}