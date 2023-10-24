#include <gtest/gtest.h>
#include "src/lexer.hh"
#include "src/parser.hh"

TEST(ParserTest, ItShouldParseFunctionDefinition)
{
    std::string sourceCode = "function integer foo() {return 3 + 5;};";

    std::deque<lexer::Token> tokens = lexer::lex(sourceCode);

    parser::Parser testObject(tokens);

    parser::Program program = testObject.parse();

    EXPECT_EQ(1, program.stmts.size());

    std::shared_ptr<parser::FunctionStmt> functionStmt = program.get<parser::FunctionStmt>(0);
    EXPECT_EQ("foo", functionStmt->identifier);
    EXPECT_EQ(parser::Type::INTEGER, functionStmt->returnType);
    EXPECT_EQ(parser::StmtType::FUNCTION, functionStmt.get()->type);

    EXPECT_EQ(1, functionStmt->stmts.size());

    std::shared_ptr<parser::ReturnStmt> returnStmt = std::static_pointer_cast<parser::ReturnStmt>(functionStmt->stmts[0]);
    EXPECT_EQ(parser::Type::INTEGER, returnStmt->expr->returnType);
}

TEST(ParserTest, ItShouldParseFunctionDefinitionWithOnlyOneInteger)
{
    std::string sourceCode = "function integer foo() {return 3;};";

    std::deque<lexer::Token> tokens = lexer::lex(sourceCode);

    parser::Parser testObject(tokens);

    parser::Program program = testObject.parse();

    EXPECT_EQ(1, program.stmts.size());

    std::shared_ptr<parser::FunctionStmt> functionStmt = program.get<parser::FunctionStmt>(0);
    EXPECT_EQ("foo", functionStmt->identifier);
    EXPECT_EQ(parser::Type::INTEGER, functionStmt->returnType);
    EXPECT_EQ(parser::StmtType::FUNCTION, functionStmt.get()->type);

    EXPECT_EQ(1, functionStmt->stmts.size());

    std::shared_ptr<parser::ReturnStmt> returnStmt = std::static_pointer_cast<parser::ReturnStmt>(functionStmt->stmts[0]);
    EXPECT_EQ(parser::Type::INTEGER, returnStmt->expr->returnType);
    std::shared_ptr<parser::IntegerLiteral> expr = std::static_pointer_cast<parser::IntegerLiteral>(returnStmt->expr);
    EXPECT_EQ(3, expr->integer);
}

TEST(ParserTest, ItShouldBeAbleToParserMainWithAPrint)
{
    std::string sourceCode = "function void bar() {print (\"Hello, World!\");};";

    std::deque<lexer::Token> tokens = lexer::lex(sourceCode);

    parser::Parser testObject(tokens);
    parser::Program program = testObject.parse();

    EXPECT_EQ(1, program.stmts.size());

    std::shared_ptr<parser::FunctionStmt> functionStmt = program.get<parser::FunctionStmt>(0);
    EXPECT_EQ("bar", functionStmt->identifier);
    EXPECT_EQ(parser::StmtType::FUNCTION, functionStmt.get()->type);

    EXPECT_EQ(1, functionStmt->stmts.size());

    std::shared_ptr<parser::Stmt> printStmt = functionStmt->stmts[0];

    EXPECT_EQ(parser::StmtType::PRINT, printStmt->type);

    parser::PrintStmt *printStmtPointer = (parser::PrintStmt *)printStmt.get();
    parser::StringLiteral *stringLiteralPointer = (parser::StringLiteral *)printStmtPointer->expr.get();

    EXPECT_EQ(stringLiteralPointer->type, parser::ExprType::STRING_LITERAL);
    EXPECT_EQ("Hello, World!", stringLiteralPointer->literal);
    EXPECT_EQ(parser::Type::STRING, stringLiteralPointer->returnType);
}

TEST(ParserTest, ItShouldBeAbleToParserPrintWithEmptyStringBody)
{
    std::string sourceCode = "function void foo() {print (\"\");};";

    std::deque<lexer::Token> tokens = lexer::lex(sourceCode);

    parser::Parser testObject(tokens);
    parser::Program program = testObject.parse();

    EXPECT_EQ(1, program.stmts.size());

    std::shared_ptr<parser::FunctionStmt> functionStmt = program.get<parser::FunctionStmt>(0);
    EXPECT_EQ("foo", functionStmt->identifier);
    EXPECT_EQ(parser::StmtType::FUNCTION, functionStmt.get()->type);

    EXPECT_EQ(1, functionStmt->stmts.size());

    std::shared_ptr<parser::Stmt> printStmt = functionStmt->stmts[0];

    EXPECT_EQ(parser::StmtType::PRINT, printStmt->type);

    parser::PrintStmt *printStmtPointer = (parser::PrintStmt *)printStmt.get();
    parser::StringLiteral *stringLiteralPointer = (parser::StringLiteral *)printStmtPointer->expr.get();

    EXPECT_EQ(stringLiteralPointer->type, parser::ExprType::STRING_LITERAL);
    EXPECT_EQ("", stringLiteralPointer->literal);
}

TEST(ParserTest, ItShouldBeAbleToPrintAndRecoverFromBadStmt)
{
    std::string sourceCode =
        R"(
    function void foo() {
        print"Hello World!");
        print("Hello World!");
    };)";

    std::deque<lexer::Token> tokens = lexer::lex(sourceCode);

    parser::Parser testObject(tokens);
    parser::Program program = testObject.parse();

    EXPECT_EQ(1, program.stmts.size());
    EXPECT_FALSE(program.isSyntacticallyCorrect());

    std::vector<parser::ErrorLog> errors = program.errors;

    EXPECT_EQ(1, errors.size());

    parser::ErrorLog error = errors[0];
    EXPECT_EQ("Expected: LEFT_PAREN at line 3, column 14, but found \"\"Hello World!\"\".", error.getMessage());

    std::shared_ptr<parser::FunctionStmt> stmt = program.get<parser::FunctionStmt>(0);
    EXPECT_EQ(2, stmt->stmts.size());

    parser::BadStmt *badStmt = (parser::BadStmt *)stmt->stmts[0].get();
    EXPECT_EQ(parser::StmtType::BAD, badStmt->type);

    parser::PrintStmt *printStmt = (parser::PrintStmt *)stmt->stmts[1].get();
    EXPECT_EQ(parser::StmtType::PRINT, printStmt->type);
}

TEST(ParserTest, ItShouldBeAbleToHandleGibberish)
{
    std::string sourceCode =
    R"(
        This is essentially gibberish
    )";

    std::deque<lexer::Token> tokens = lexer::lex(sourceCode);

    parser::Parser testObject(tokens);
    parser::Program program = testObject.parse();

    EXPECT_FALSE(program.isSyntacticallyCorrect());
}

TEST(ParserTest, ItShouldBeAbleToConstructAnInvalidSyntaxException)
{
    lexer::Token offender(lexer::TokenType::EQUALS, "=", lexer::Location(1, 1), lexer::Location(1, 2));
    std::vector<lexer::TokenType> expected{lexer::TokenType::FUNCTION};

    parser::InvalidSyntaxException ise(offender, expected);

    EXPECT_STREQ("Expected: FUNCTION at line 1, column 1, but found \"=\".", ise.what());
}

TEST(ParserTest, ItShouldNotBeAbleToConstructAnISEWithoutExpectedTokenTypes)
{
    lexer::Token offender(lexer::TokenType::EQUALS, "=", lexer::Location(1, 1), lexer::Location(1, 2));
    std::vector<lexer::TokenType> expected;

    try
    {
        parser::InvalidSyntaxException ise(offender, expected);
        FAIL() << "Should have thrown std::invalid_argument.";
    }
    catch (std::invalid_argument &e)
    {
        EXPECT_STREQ("Cannot construct InvalidSyntaxException with empty list of expected tokens types.", e.what());
    }
    catch (...)
    {
        FAIL() << "Should have thrown std::invalid_argument.";
    }
}

TEST(ParserTest, ItShouldThrowExceptionIfOffenderTokenTypeExistsWithinExpected)
{
    lexer::Token offender(lexer::TokenType::EQUALS, "=", lexer::Location(1, 1), lexer::Location(1, 2));
    std::vector<lexer::TokenType> expected{lexer::TokenType::EQUALS};

    try
    {
        parser::InvalidSyntaxException ise(offender, expected);
        FAIL() << "Should have thrown std::invalid_argument.";
    }
    catch (std::invalid_argument &e)
    {
        EXPECT_STREQ("Cannot construct InvalidSyntaxException where expected tokens contains offender EQUALS [EQUALS].", e.what());
    }
    catch (...)
    {
        FAIL() << "Should have thrown std::invalid_argument.";
    }
}
