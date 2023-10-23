#include "parser.hh"

#include <vector>
#include <deque>
#include "lexer.hh"
#include <iostream>
#include <memory>
#include <iostream>
#include <string>
#include <algorithm>
#include "util.hh"
#include <map>
#include <string_view>

namespace parser
{
    ReturnStmt::ReturnStmt(std::shared_ptr<Expr> expr) : expr(std::move(expr))
    {
    }

    PrintStmt::PrintStmt(std::shared_ptr<Expr> expr) : expr(std::move(expr))
    {
    }
    
    BadStmt::BadStmt(lexer::Token token, std::vector<lexer::TokenType> expected, std::string message) : offender(token), expected(expected), message(message)
    {
        
    }

    std::string InvalidSyntaxException::parseMessage(lexer::Token offender, std::vector<lexer::TokenType> expected)
    {
        auto asString = [](std::vector<lexer::TokenType> expected)
        {
            std::vector<std::string> tokenTypeToStrings;
            for (auto &tokenType : expected)
            {
                tokenTypeToStrings.push_back(lexer::tostring(tokenType));
            }
            return util::join(tokenTypeToStrings.begin(), tokenTypeToStrings.end(), ", ");
        };

        if (expected.empty())
        {
            throw std::invalid_argument("Cannot construct InvalidSyntaxException with empty list of expected tokens types.");
        }

        if (std::find(expected.begin(), expected.end(), offender.getTokenType()) != expected.end())
        {
            throw std::invalid_argument("Cannot construct InvalidSyntaxException where expected tokens contains offender " + lexer::tostring(offender.getTokenType()) + " [" + asString(expected) + "].");
        }

        return "Expected: " + asString(expected) + " at line " + std::to_string(offender.getStart().getRow()) + ", column " + std::to_string(offender.getStart().getColumn()) + ", but found \"" + offender.getRaw() + "\".";
    }

    InvalidSyntaxException::InvalidSyntaxException(lexer::Token offender, std::vector<lexer::TokenType> expected) : std::runtime_error(parser::InvalidSyntaxException::parseMessage(offender, expected)), offender(offender), expected(expected)
    {
    }

    ErrorLog::ErrorLog(std::string message) : message(message)
    {
    }

    const char *InvalidSyntaxException::what() const throw()
    {
        return std::runtime_error::what();
    }

    std::string ErrorLog::getMessage()
    {
        return this->message;
    }

    bool Program::isSyntacticallyCorrect()
    {
        return this->errors.empty();
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

        this->compiling.stmts = stmts;
        return compiling;
    };

    std::shared_ptr<Stmt> Parser::stmt()
    {
        lexer::Token maybeReturnOrFunctionDefinition = this->peek();

        try
        {
            if (maybeReturnOrFunctionDefinition.getTokenType() == lexer::TokenType::RETURN)
            {
                return this->returnStmt();
            }
            else if (maybeReturnOrFunctionDefinition.getTokenType() == lexer::TokenType::PRINT)
            {
                return this->printStmt();
            }
            else
            {
                return this->functionStmt();
            }
        }
        catch (parser::InvalidSyntaxException &ise) 
        {
            auto syncWithSemicolon = [&]() {
                while (this->pop().getTokenType() != lexer::TokenType::SEMICOLON);
            };

            this->compiling.errors.push_back(parser::ErrorLog(ise.what()));
            std::shared_ptr<Stmt> badStmt = std::make_shared<parser::BadStmt>(ise.offender, ise.expected, ise.what());
            badStmt->type = parser::StmtType::BAD;

            syncWithSemicolon();

            return badStmt;
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

    std::shared_ptr<Stmt> Parser::printStmt()
    {
        this->consume(lexer::TokenType::PRINT);

        this->consume(lexer::TokenType::LEFT_PAREN);
        parser::PrintStmt *printStmtPointer = new parser::PrintStmt(this->expr());
        printStmtPointer->type = parser::StmtType::PRINT;
        this->consume(lexer::TokenType::RIGHT_PAREN);

        std::shared_ptr<parser::Stmt> printStmt(printStmtPointer);
        this->consume(lexer::TokenType::SEMICOLON);

        return printStmt;
    }

    std::shared_ptr<Stmt> Parser::returnStmt()
    {
        this->consume(lexer::TokenType::RETURN);

        parser::ReturnStmt *returnStmtPointer = new parser::ReturnStmt(this->expr());
        returnStmtPointer->type = parser::StmtType::RETURN;
        std::shared_ptr<parser::Stmt> returnStmt(returnStmtPointer);
        this->consume(lexer::TokenType::SEMICOLON);
        return returnStmt;
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
        lexer::Token consumed = this->peek();
        if (consumed.getTokenType() != tokenType)
        {
            throw parser::InvalidSyntaxException(consumed, std::vector<lexer::TokenType>{tokenType});
        }
        else
        {
            this->pop();
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
        lexer::Token peeked = this->peek();
        if (peeked.getTokenType() == lexer::TokenType::STRING)
        {
            return this->parseStringLiteral();
        }
        else
        {
            return this->parseBinaryOperation();
        }
    }

    std::shared_ptr<parser::Expr> Parser::parseStringLiteral()
    {
        lexer::Token popped = this->pop();
        parser::StringLiteral *stringLiteral = new parser::StringLiteral();
        stringLiteral->literal = popped.getRaw().substr(1, popped.getRaw().length() - 2);
        stringLiteral->type = parser::ExprType::STRING_LITERAL;
        return std::shared_ptr<parser::Expr>(stringLiteral);
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