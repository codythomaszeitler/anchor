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
    
    Context::Context()
    {
        this->parent = nullptr;
    }
    
    void Context::setType(std::string identifier, parser::Type type)
    {
        this->varIdToVarType[identifier] = type;
    }
    
    parser::Type Context::getType(std::string identifier)
    {
        Context* iterator = this;
        while (iterator != nullptr) 
        {
            if (iterator->varIdToVarType.contains(identifier))
            {
                return iterator->varIdToVarType[identifier];
            }
            iterator = iterator->parent;
        }
        return parser::Type::NOT_FOUND;
    }

    Parser::Parser(std::deque<lexer::Token> tokens) : tokens(tokens)
    {
    }

    parser::Program Parser::parse()
    {
        std::vector<std::shared_ptr<Stmt>> stmts;
        while (this->peek().getTokenType() != lexer::TokenType::END_OF_STREAM && !this->tokens.empty())
        {
            stmts.push_back(this->stmt());
        }

        this->compiling.stmts = stmts;
        return compiling;
    };

    std::shared_ptr<Stmt> Parser::stmt()
    {
        auto isType = [](lexer::TokenType tokenType)
        {
            return tokenType == lexer::TokenType::INTEGER_TYPE || tokenType == lexer::TokenType::BOOLEAN_TYPE || tokenType == lexer::TokenType::STRING_TYPE;
        };

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
            else if (maybeReturnOrFunctionDefinition.getTokenType() == lexer::TokenType::IF)
            {
                return this->ifStmt();
            }
            else if (maybeReturnOrFunctionDefinition.getTokenType() == lexer::TokenType::WHILE)
            {
                return this->whileStmt();
            }
            else if (isType(maybeReturnOrFunctionDefinition.getTokenType()))
            {
                return this->varDeclStmt();
            }
            else if (maybeReturnOrFunctionDefinition.getTokenType() == lexer::TokenType::FUNCTION)
            {
                return this->functionStmt();
            }
            else
            {
                return this->varAssignmentStmtOrExpr();
            }
        }
        catch (parser::InvalidSyntaxException &ise)
        {
            auto syncWithSemicolon = [&]()
            {
                while (true)
                {
                    lexer::Token popped = this->pop();
                    if (popped.getTokenType() == lexer::TokenType::SEMICOLON || popped.getTokenType() == lexer::TokenType::END_OF_STREAM)
                    {
                        break;
                    }
                }
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

        parser::Type returnType = this->parseReturnType();
        std::string identifier = this->identifier();

        parser::Context parent = this->context;
        this->context = parser::Context();
        this->context.parent = &parent;

        std::vector<std::shared_ptr<parser::FunctionArgStmt>> args = this->args();

        parser::FunctionStmt *functionStmt = new parser::FunctionStmt();
        functionStmt->type = parser::StmtType::FUNCTION;
        functionStmt->returnType = returnType;
        functionStmt->stmts = this->block();
        functionStmt->identifier = identifier;
        functionStmt->args = args;

        this->context = parent;

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

    std::shared_ptr<Stmt> Parser::ifStmt()
    {
        std::shared_ptr<parser::IfStmt> ifStmt = std::make_shared<parser::IfStmt>();
        this->consume(lexer::TokenType::IF);

        this->consume(lexer::TokenType::LEFT_PAREN);
        ifStmt->condition = this->expr();
        this->consume(lexer::TokenType::RIGHT_PAREN);

        ifStmt->stmts = this->block();
        this->consume(lexer::TokenType::SEMICOLON);

        ifStmt->type = parser::StmtType::IF;
        return std::static_pointer_cast<parser::Stmt>(ifStmt);
    }

    std::shared_ptr<Stmt> Parser::whileStmt()
    {
        std::shared_ptr<parser::WhileStmt> whileStmt = std::make_shared<parser::WhileStmt>();
        whileStmt->type = parser::StmtType::WHILE;

        this->consume(lexer::TokenType::WHILE);

        this->consume(lexer::TokenType::LEFT_PAREN);
        whileStmt->condition = this->expr();
        this->consume(lexer::TokenType::RIGHT_PAREN);

        whileStmt->stmts = this->block();
        this->consume(lexer::TokenType::SEMICOLON);

        return std::static_pointer_cast<parser::Stmt>(whileStmt);
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

    std::shared_ptr<Stmt> Parser::varDeclStmt()
    {
        parser::Type type = this->type();
        std::string identifier = this->identifier();

        this->consume(lexer::TokenType::SEMICOLON);

        std::shared_ptr<parser::VarDeclStmt> varDeclStmt = std::make_shared<parser::VarDeclStmt>();
        varDeclStmt->identifier = identifier;
        varDeclStmt->type = parser::StmtType::VAR_DECL;
        varDeclStmt->variableType = type;

        this->context.setType(varDeclStmt->identifier, varDeclStmt->variableType);

        return varDeclStmt;
    }

    std::shared_ptr<Stmt> Parser::varAssignmentStmtOrExpr()
    {
        std::shared_ptr<parser::Expr> expr = this->expr();

        std::shared_ptr<parser::ExprStmt> exprStmt = std::make_unique<parser::ExprStmt>();
        exprStmt->type = parser::StmtType::EXPR;
        exprStmt->expr = expr;

        this->consume(lexer::TokenType::SEMICOLON);

        return exprStmt;
    }

    std::string Parser::identifier()
    {
        lexer::Token token = this->pop();
        if (token.getTokenType() != lexer::TokenType::IDENTIFIER)
        {
            throw parser::InvalidSyntaxException(token, std::vector<lexer::TokenType>{lexer::TokenType::IDENTIFIER});
        }
        return token.getRaw();
    }
    
    parser::Type Parser::type()
    {
        lexer::Token variableType = this->pop();
        if (variableType.getTokenType() == lexer::TokenType::INTEGER_TYPE)
        {
            return parser::Type::INTEGER;
        }
        else if (variableType.getTokenType() == lexer::TokenType::BOOLEAN_TYPE)
        {
            return parser::Type::BOOLEAN;
        }
        else if (variableType.getTokenType() == lexer::TokenType::STRING_TYPE)
        {
            return parser::Type::STRING;
        }
        else if (variableType.getTokenType() == lexer::TokenType::VOID_TYPE)
        {
            return parser::Type::VOID;
        }
        else
        {
            throw parser::InvalidSyntaxException(variableType, std::vector<lexer::TokenType>{lexer::TokenType::INTEGER_TYPE, lexer::TokenType::BOOLEAN_TYPE});
        }
    }

    parser::Type Parser::parseReturnType()
    {
        return this->type(); 
    }

    std::vector<std::shared_ptr<parser::FunctionArgStmt>> Parser::args()
    {
        this->consume(lexer::TokenType::LEFT_PAREN);

        std::vector<std::shared_ptr<parser::FunctionArgStmt>> args;
        while (this->peek().getTokenType() != lexer::TokenType::RIGHT_PAREN)
        {
            parser::Type type = this->type();
            std::string identifier = this->identifier();

            std::shared_ptr<parser::FunctionArgStmt> arg = std::make_shared<parser::FunctionArgStmt>();
            arg->type = parser::StmtType::FUNCTION_ARG;
            arg->identifier = identifier;
            arg->returnType = type;

            this->context.setType(arg->identifier, arg->returnType);

            args.push_back(arg);

            if (this->peek().getTokenType() == lexer::TokenType::SEMICOLON)
            {
                this->consume(lexer::TokenType::SEMICOLON);
            }
        }

        this->consume(lexer::TokenType::RIGHT_PAREN);
        return args;
    }

    std::vector<std::shared_ptr<Stmt>> Parser::block()
    {
        this->consume(lexer::TokenType::LEFT_BRACKET);
        std::vector<std::shared_ptr<Stmt>> stmts;

        parser::Context parent = this->context;

        this->context = parser::Context();
        this->context.parent = &parent;
        while (this->peek().getTokenType() != lexer::TokenType::RIGHT_BRACKET)
        {
            stmts.push_back(this->stmt());
        }

        this->context = parent;

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
        auto isBinaryOp = [](lexer::TokenType tokenType)
        {
            return tokenType == lexer::TokenType::PLUS_SIGN ||
                   tokenType == lexer::TokenType::MINUS_SIGN ||
                   tokenType == lexer::TokenType::MULT_SIGN;
        };

        auto isBooleanBinaryOp = [](lexer::TokenType tokenType)
        {
            return tokenType == lexer::TokenType::LESS_THAN_SIGN || tokenType == lexer::TokenType::GREATER_THAN_SIGN || tokenType == lexer::TokenType::DOUBLE_EQUALS;
        };

        lexer::Token peeked = this->peek();

        std::shared_ptr<parser::Expr> lhs;
        if (peeked.getTokenType() == lexer::TokenType::STRING)
        {
            lhs = this->parseStringLiteral();
        }
        else if (peeked.getTokenType() == lexer::TokenType::IDENTIFIER)
        {
            lhs = this->parseFunctionOrVarExpr();
        }
        else if (peeked.getTokenType() == lexer::TokenType::INTEGER)
        {
            lhs = this->parseInteger();
        }
        else if (peeked.getTokenType() == lexer::TokenType::TRUE ||
                 peeked.getTokenType() == lexer::TokenType::FALSE)
        {
            lhs = this->parseBoolean();
        }

        if (isBinaryOp(this->peek().getTokenType()))
        {
            parser::BinaryOperation *binaryOperation = new parser::BinaryOperation();
            binaryOperation->left = lhs;
            binaryOperation->operation = this->parseOperation();
            binaryOperation->right = this->expr();
            binaryOperation->type = parser::ExprType::BINARY_OP;
            binaryOperation->returnType = parser::Type::INTEGER;
            return std::shared_ptr<parser::Expr>(binaryOperation);
        }

        if (isBooleanBinaryOp(this->peek().getTokenType()))
        {
            std::shared_ptr<parser::BinaryOperation> binaryOperation = std::make_shared<parser::BinaryOperation>();
            binaryOperation->left = lhs;
            binaryOperation->operation = this->parseOperation();
            binaryOperation->right = this->expr();
            binaryOperation->type = parser::ExprType::BINARY_OP;
            binaryOperation->returnType = parser::Type::BOOLEAN;
            return binaryOperation;
        }

        return lhs;
    }

    std::shared_ptr<parser::Expr> Parser::parseStringLiteral()
    {
        lexer::Token popped = this->pop();
        parser::StringLiteral *stringLiteral = new parser::StringLiteral();
        stringLiteral->literal = popped.getRaw().substr(1, popped.getRaw().length() - 2);
        stringLiteral->type = parser::ExprType::STRING_LITERAL;
        stringLiteral->returnType = parser::Type::STRING;
        return std::shared_ptr<parser::Expr>(stringLiteral);
    }

    std::shared_ptr<parser::Expr> Parser::parseFunctionOrVarExpr()
    {
        std::string identifier = this->identifier();
        if (this->peek().getTokenType() == lexer::TokenType::LEFT_PAREN)
        {
            parser::FunctionExpr *functionExpr = new parser::FunctionExpr();
            functionExpr->identifier = identifier;
            functionExpr->type = parser::ExprType::FUNCTION;
            functionExpr->returnType = parser::Type::INTEGER;

            this->consume(lexer::TokenType::LEFT_PAREN);

            while (this->peek().getTokenType() != lexer::TokenType::RIGHT_PAREN)
            {
                std::shared_ptr<parser::Expr> arg = this->expr();
                functionExpr->args.push_back(arg);

                if (this->peek().getTokenType() == lexer::TokenType::SEMICOLON)
                {
                    this->consume(lexer::TokenType::SEMICOLON);
                }
            }

            this->consume(lexer::TokenType::RIGHT_PAREN);

            return std::shared_ptr<Expr>(functionExpr);
        }
        else if (this->peek().getTokenType() == lexer::TokenType::EQUALS)
        {
            this->consume(lexer::TokenType::EQUALS);

            std::shared_ptr<parser::VarAssignmentExpr> varAssignmentExpr = std::make_shared<parser::VarAssignmentExpr>();
            varAssignmentExpr->type = parser::ExprType::ASSIGNMENT;
            varAssignmentExpr->returnType = this->context.getType(identifier);
            varAssignmentExpr->expr = this->expr();
            varAssignmentExpr->identifier = identifier;

            return std::static_pointer_cast<parser::Expr>(varAssignmentExpr);
        }
        else
        {
            std::shared_ptr<parser::VarExpr> varExpr = std::make_shared<parser::VarExpr>();
            varExpr->identifier = identifier;
            varExpr->returnType = this->context.getType(identifier);
            varExpr->type = parser::ExprType::VAR;
            return std::static_pointer_cast<parser::Expr>(varExpr);
        }
    }

    std::shared_ptr<parser::Expr> Parser::parseInteger()
    {
        lexer::Token integerToken = this->pop();
        std::string intAsString = integerToken.getRaw();

        parser::IntegerLiteral *integerLiteral = new parser::IntegerLiteral();
        integerLiteral->integer = stoi(intAsString);
        integerLiteral->type = parser::ExprType::INTEGER_LITERAL;
        integerLiteral->returnType = parser::Type::INTEGER;
        return std::shared_ptr<parser::Expr>(integerLiteral);
    }

    std::shared_ptr<parser::Expr> Parser::parseBoolean()
    {
        lexer::Token booleanPrimitive = this->pop();

        std::shared_ptr<parser::BooleanLiteralExpr> booleanLiteralExpr = std::make_shared<parser::BooleanLiteralExpr>();
        booleanLiteralExpr->type = parser::ExprType::BOOLEAN;
        booleanLiteralExpr->returnType = parser::Type::BOOLEAN;

        if (booleanPrimitive.getTokenType() == lexer::TokenType::TRUE)
        {
            booleanLiteralExpr->value = true;
            return std::static_pointer_cast<parser::Expr>(booleanLiteralExpr);
        }
        else if (booleanPrimitive.getTokenType() == lexer::TokenType::FALSE)
        {
            booleanLiteralExpr->value = false;
            return std::static_pointer_cast<parser::Expr>(booleanLiteralExpr);
        }
        else
        {
            throw parser::InvalidSyntaxException(booleanPrimitive, std::vector<lexer::TokenType>{lexer::TokenType::TRUE, lexer::TokenType::FALSE});
        }
    }

    parser::Operation Parser::parseOperation()
    {
        lexer::Token operation = this->pop();

        if (operation.getTokenType() == lexer::TokenType::PLUS_SIGN)
        {
            return parser::Operation::ADD;
        }
        else if (operation.getTokenType() == lexer::TokenType::MINUS_SIGN)
        {
            return parser::Operation::SUBTRACT;
        }
        else if (operation.getTokenType() == lexer::TokenType::MULT_SIGN)
        {
            return parser::Operation::MULTIPLICATION;
        }
        else if (operation.getTokenType() == lexer::TokenType::LESS_THAN_SIGN)
        {
            return parser::Operation::LESS_THAN;
        }
        else if (operation.getTokenType() == lexer::TokenType::GREATER_THAN_SIGN)
        {
            return parser::Operation::GREATER_THAN;
        }
        else if (operation.getTokenType() == lexer::TokenType::EQUALS)
        {
            return parser::Operation::ASSIGNMENT;
        }
        else if (operation.getTokenType() == lexer::TokenType::DOUBLE_EQUALS)
        {
            return parser::Operation::EQUALS;
        }
        else
        {
            throw parser::InvalidSyntaxException(operation, std::vector<lexer::TokenType>{lexer::TokenType::PLUS_SIGN, lexer::TokenType::MINUS_SIGN});
        }
    }
}