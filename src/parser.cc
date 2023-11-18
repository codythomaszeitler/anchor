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
#include <iterator>

namespace parser
{
    std::string tostring(parser::Type type)
    {
        static std::map<parser::Type, std::string> map;
        if (map.empty())
        {
            using enum parser::Type;
            map[VOID] = "VOID";
            map[INTEGER] = "INTEGER";
            map[STRING] = "STRING";
            map[BOOLEAN] = "BOOLEAN";
        }

        return map[type];
    }

    ReturnStmt::ReturnStmt(std::shared_ptr<Expr> expr) : expr(std::move(expr))
    {
    }

    PrintStmt::PrintStmt(std::shared_ptr<Expr> expr) : expr(std::move(expr))
    {
    }

    BadStmt::BadStmt(const lexer::Token &token, const std::vector<lexer::TokenType> &expected, const std::string &message) : offender(token), expected(expected), message(message)
    {
    }

    BadStmt::BadStmt(const lexer::Token &token, const std::string &message) : offender(token), message(message)
    {
    }

    std::string InvalidSyntaxException::parseMessage(const lexer::Token &offender, const std::vector<lexer::TokenType> &expected)
    {
        auto asString = [](const std::vector<lexer::TokenType> &toConvert)
        {
            std::vector<std::string> tokenTypeToStrings;
            for (const auto &tokenType : toConvert)
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

    InvalidSyntaxException::InvalidSyntaxException(const lexer::Token &offender, const std::vector<lexer::TokenType> &expected) : std::runtime_error(parser::InvalidSyntaxException::parseMessage(offender, expected)), offender(offender), expected(expected)
    {
    }

    std::string InvalidTypeException::parseMessage(const lexer::Token &offender, std::shared_ptr<parser::BinaryOperation> binaryOp)
    {
        return "Expected: " + parser::tostring(binaryOp->left->returnType) + " at line " + std::to_string(offender.getStart().getRow()) + ", column " + std::to_string(offender.getStart().getColumn()) + ", but found " + parser::tostring(binaryOp->right->returnType) + ".";
    }

    InvalidTypeException::InvalidTypeException(const lexer::Token &offender, std::shared_ptr<parser::BinaryOperation> binaryOp) : std::runtime_error(parser::InvalidTypeException::parseMessage(offender, binaryOp)), offender(offender), binaryOp(binaryOp)
    {
    }

    ErrorLog::ErrorLog(const std::string &message) : message(message)
    {
    }

    const char *InvalidSyntaxException::what() const throw()
    {
        return std::runtime_error::what();
    }

    const char *InvalidTypeException::what() const throw()
    {
        return std::runtime_error::what();
    }

    std::string ErrorLog::getMessage() const
    {
        return this->message;
    }

    bool Program::isSyntacticallyCorrect() const
    {
        return this->errors.empty();
    }

    void Context::setParent(parser::Context *newParent)
    {
        this->parent = newParent;
    }

    void Context::setType(const std::string &identifier, parser::Type type)
    {
        this->varIdToVarType[identifier] = type;
    }

    parser::Type Context::getType(const std::string &identifier)
    {
        Context *iterator = this;
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
    
    void Context::setFunctionType(const std::string& identifier, parser::Type type)
    {
        this->functionIdToType[identifier] = type;
    }
    
    parser::Type Context::getFunctionType(const std::string& identifier)
    {
        Context *iterator = this;
        while (iterator != nullptr)
        {
            if (iterator->functionIdToType.contains(identifier))
            {
                return iterator->functionIdToType[identifier];
            }
            iterator = iterator->parent;
        }
        return parser::Type::NOT_FOUND;
    }

    Parser::Parser(const std::deque<lexer::Token> &tokens) : tokens(tokens)
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
        using enum lexer::TokenType;
        auto isType = [](lexer::TokenType tokenType)
        {
            return tokenType == INTEGER_TYPE || tokenType == BOOLEAN_TYPE || tokenType == STRING_TYPE;
        };

        auto syncWithSemicolon = [&]()
        {
            while (true)
            {
                lexer::Token popped = this->pop();
                if (popped.getTokenType() == SEMICOLON || popped.getTokenType() == END_OF_STREAM)
                {
                    break;
                }
            }
        };

        lexer::Token maybeReturnOrFunctionDefinition = this->peek();

        try
        {
            if (maybeReturnOrFunctionDefinition.getTokenType() == RETURN)
            {
                return this->returnStmt();
            }
            else if (maybeReturnOrFunctionDefinition.getTokenType() == PRINT)
            {
                return this->printStmt();
            }
            else if (maybeReturnOrFunctionDefinition.getTokenType() == IF)
            {
                return this->ifStmt();
            }
            else if (maybeReturnOrFunctionDefinition.getTokenType() == WHILE)
            {
                return this->whileStmt();
            }
            else if (isType(maybeReturnOrFunctionDefinition.getTokenType()))
            {
                return this->varDeclStmt();
            }
            else if (maybeReturnOrFunctionDefinition.getTokenType() == FUNCTION)
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
            this->compiling.errors.emplace_back(ise.what());
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
        this->context.setParent(&parent);

        std::vector<std::shared_ptr<parser::FunctionArgStmt>> args = this->args();

        auto functionStmt = std::make_shared<parser::FunctionStmt>();
        functionStmt->type = parser::StmtType::FUNCTION;
        functionStmt->returnType = returnType;
        functionStmt->stmts = this->block();
        functionStmt->identifier = identifier;
        functionStmt->args = args;

        this->context = parent;
        this->context.setFunctionType(functionStmt->identifier, functionStmt->returnType);

        this->consume(lexer::TokenType::SEMICOLON);
        return functionStmt;
    }

    std::shared_ptr<Stmt> Parser::printStmt()
    {
        this->consume(lexer::TokenType::PRINT);

        this->consume(lexer::TokenType::LEFT_PAREN);

        lexer::Token peeked = this->peek();
        std::shared_ptr<parser::Expr> expr = this->expr();
        auto printStmt = std::make_shared<parser::PrintStmt>(expr);
        printStmt->type = parser::StmtType::PRINT;
        this->consume(lexer::TokenType::RIGHT_PAREN);

        this->consume(lexer::TokenType::SEMICOLON);

        if (parser::Expr::hasTypeError(expr))
        {
            this->compiling.errors.emplace_back(parser::Expr::getTypeErrorMessage(peeked, expr));
        }

        return printStmt;
    }

    std::shared_ptr<Stmt> Parser::ifStmt()
    {
        auto ifStmt = std::make_shared<parser::IfStmt>();
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
        auto whileStmt = std::make_shared<parser::WhileStmt>();
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

        auto returnStmt = std::make_shared<parser::ReturnStmt>(this->expr());
        returnStmt->type = parser::StmtType::RETURN;

        this->consume(lexer::TokenType::SEMICOLON);
        return returnStmt;
    }

    std::shared_ptr<Stmt> Parser::varDeclStmt()
    {
        parser::Type type = this->type();
        std::string identifier = this->identifier();

        this->consume(lexer::TokenType::SEMICOLON);

        auto varDeclStmt = std::make_shared<parser::VarDeclStmt>();
        varDeclStmt->identifier = identifier;
        varDeclStmt->type = parser::StmtType::VAR_DECL;
        varDeclStmt->variableType = type;

        this->context.setType(varDeclStmt->identifier, varDeclStmt->variableType);

        return varDeclStmt;
    }

    std::shared_ptr<Stmt> Parser::varAssignmentStmtOrExpr()
    {
        lexer::Token peeked = this->peek();
        std::shared_ptr<parser::Expr> expr = this->expr();

        std::shared_ptr<parser::ExprStmt> exprStmt = std::make_unique<parser::ExprStmt>();
        exprStmt->type = parser::StmtType::EXPR;
        exprStmt->expr = expr;

        this->consume(lexer::TokenType::SEMICOLON);

        if (parser::Expr::hasTypeError(expr))
        {
            this->compiling.errors.emplace_back(parser::Expr::getTypeErrorMessage(peeked, expr));
        }

        return exprStmt;
    }

    bool Expr::hasTypeError(std::shared_ptr<parser::Expr> expr)
    {

        if (expr->type == parser::ExprType::BINARY_OP)
        {
            std::shared_ptr<parser::BinaryOperation> binaryOp = std::static_pointer_cast<parser::BinaryOperation>(expr);

            return binaryOp->left->returnType != binaryOp->right->returnType;
        }
        else if (expr->type == parser::ExprType::ASSIGNMENT)
        {
            std::shared_ptr<parser::VarAssignmentExpr> varAssignmentExpr = std::static_pointer_cast<parser::VarAssignmentExpr>(expr);

            return varAssignmentExpr->returnType != varAssignmentExpr->expr->returnType;
        }

        return false;
    }

    std::string Expr::getTypeErrorMessage(const lexer::Token &peeked, std::shared_ptr<parser::Expr> expr)
    {
        auto parseErrorMessage = [&peeked](std::shared_ptr<parser::Expr> left, std::shared_ptr<parser::Expr> right)
        {
            return "Type Error: Expression at line " + std::to_string(peeked.getStart().getRow()) + ", column " + std::to_string(peeked.getStart().getColumn()) + " had " + parser::tostring(left->returnType) + " on left, " + parser::tostring(right->returnType) + " on right.";
        };

        if (expr->type == parser::ExprType::BINARY_OP)
        {
            std::shared_ptr<parser::BinaryOperation> binaryOp = std::static_pointer_cast<parser::BinaryOperation>(expr);

            return parseErrorMessage(binaryOp->left, binaryOp->right);
        }
        else if (expr->type == parser::ExprType::ASSIGNMENT)
        {
            std::shared_ptr<parser::VarAssignmentExpr> varAssignmentExpr = std::static_pointer_cast<parser::VarAssignmentExpr>(expr);
            return parseErrorMessage(varAssignmentExpr, varAssignmentExpr->expr);
        }
        return std::string("");
    };

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
        using enum lexer::TokenType;
        lexer::Token variableType = this->pop();
        if (variableType.getTokenType() == INTEGER_TYPE)
        {
            return parser::Type::INTEGER;
        }
        else if (variableType.getTokenType() == BOOLEAN_TYPE)
        {
            return parser::Type::BOOLEAN;
        }
        else if (variableType.getTokenType() == STRING_TYPE)
        {
            return parser::Type::STRING;
        }
        else if (variableType.getTokenType() == VOID_TYPE)
        {
            return parser::Type::VOID;
        }
        else
        {
            throw parser::InvalidSyntaxException(variableType, std::vector<lexer::TokenType>{INTEGER_TYPE, BOOLEAN_TYPE});
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

            auto arg = std::make_shared<parser::FunctionArgStmt>();
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
        using enum lexer::TokenType;
        this->consume(LEFT_BRACKET);
        std::vector<std::shared_ptr<Stmt>> stmts;

        parser::Context parent = this->context;

        this->context = parser::Context();
        this->context.setParent(&parent);
        while (this->peek().getTokenType() != RIGHT_BRACKET)
        {
            stmts.push_back(this->stmt());
        }

        this->context = parent;

        this->consume(RIGHT_BRACKET);
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
        using enum lexer::TokenType;
        auto isBinaryOp = [](lexer::TokenType tokenType)
        {
            return tokenType == PLUS_SIGN ||
                   tokenType == MINUS_SIGN ||
                   tokenType == MULT_SIGN;
        };

        auto isBooleanBinaryOp = [](lexer::TokenType tokenType)
        {
            return tokenType == LESS_THAN_SIGN || tokenType == GREATER_THAN_SIGN || tokenType == DOUBLE_EQUALS;
        };

        lexer::Token peeked = this->peek();

        std::shared_ptr<parser::Expr> lhs;
        if (peeked.getTokenType() == STRING)
        {
            lhs = this->parseStringLiteral();
        }
        else if (peeked.getTokenType() == IDENTIFIER)
        {
            lhs = this->parseFunctionOrVarExpr();
        }
        else if (peeked.getTokenType() == INTEGER)
        {
            lhs = this->parseInteger();
        }
        else if (peeked.getTokenType() == TRUE ||
                 peeked.getTokenType() == FALSE)
        {
            lhs = this->parseBoolean();
        }

        lexer::Token rhsPeek = this->peek();
        if (isBinaryOp(rhsPeek.getTokenType()))
        {
            auto binaryOperation = std::make_shared<parser::BinaryOperation>();
            binaryOperation->left = lhs;
            binaryOperation->operation = this->parseOperation();
            binaryOperation->right = this->expr();
            binaryOperation->type = parser::ExprType::BINARY_OP;

            if (lhs->returnType == parser::Type::STRING || binaryOperation->right->returnType == parser::Type::STRING)
            {
                binaryOperation->returnType = parser::Type::STRING;
            }
            else
            {
                binaryOperation->returnType = parser::Type::INTEGER;
            }
            return std::static_pointer_cast<parser::Expr>(binaryOperation);
        }

        if (isBooleanBinaryOp(this->peek().getTokenType()))
        {
            auto binaryOperation = std::make_shared<parser::BinaryOperation>();
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
        auto stringLiteral = std::make_shared<parser::StringLiteral>();
        stringLiteral->literal = popped.getRaw().substr(1, popped.getRaw().length() - 2);
        stringLiteral->type = parser::ExprType::STRING_LITERAL;
        stringLiteral->returnType = parser::Type::STRING;
        return stringLiteral;
    }

    std::shared_ptr<parser::Expr> Parser::parseFunctionOrVarExpr()
    {
        std::string identifier = this->identifier();
        if (this->peek().getTokenType() == lexer::TokenType::LEFT_PAREN)
        {
            auto functionExpr = std::make_shared<parser::FunctionExpr>();
            functionExpr->identifier = identifier;
            functionExpr->type = parser::ExprType::FUNCTION;
            functionExpr->returnType = this->context.getFunctionType(functionExpr->identifier);

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

            return functionExpr;
        }
        else if (this->peek().getTokenType() == lexer::TokenType::EQUALS)
        {
            this->consume(lexer::TokenType::EQUALS);

            auto varAssignmentExpr = std::make_shared<parser::VarAssignmentExpr>();
            varAssignmentExpr->type = parser::ExprType::ASSIGNMENT;
            varAssignmentExpr->returnType = this->context.getType(identifier);
            varAssignmentExpr->expr = this->expr();
            varAssignmentExpr->identifier = identifier;

            return std::static_pointer_cast<parser::Expr>(varAssignmentExpr);
        }
        else
        {
            auto varExpr = std::make_shared<parser::VarExpr>();
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

        auto integerLiteral = std::make_shared<parser::IntegerLiteral>();
        integerLiteral->integer = stoi(intAsString);
        integerLiteral->type = parser::ExprType::INTEGER_LITERAL;
        integerLiteral->returnType = parser::Type::INTEGER;
        return std::static_pointer_cast<parser::Expr>(integerLiteral);
    }

    std::shared_ptr<parser::Expr> Parser::parseBoolean()
    {
        using enum lexer::TokenType;
        lexer::Token booleanPrimitive = this->pop();

        auto booleanLiteralExpr = std::make_shared<parser::BooleanLiteralExpr>();
        booleanLiteralExpr->type = parser::ExprType::BOOLEAN;
        booleanLiteralExpr->returnType = parser::Type::BOOLEAN;

        if (booleanPrimitive.getTokenType() == TRUE)
        {
            booleanLiteralExpr->value = true;
            return std::static_pointer_cast<parser::Expr>(booleanLiteralExpr);
        }
        else if (booleanPrimitive.getTokenType() == FALSE)
        {
            booleanLiteralExpr->value = false;
            return std::static_pointer_cast<parser::Expr>(booleanLiteralExpr);
        }
        else
        {
            throw parser::InvalidSyntaxException(booleanPrimitive, std::vector<lexer::TokenType>{TRUE, FALSE});
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