#include <gtest/gtest.h>
#include "src/lexer.hh"
#include "src/parser.hh"

TEST(ParserTest, ItShouldParseFunctionDefinition)
{
    std::string sourceCode = "function foo() {return 3 + 5;};";

    std::deque<lexer::Token> tokens = lexer::lex(sourceCode);

    parser::Parser testObject(tokens);

    parser::Program program = testObject.parse();

    EXPECT_EQ(1, program.stmts.size());

    std::shared_ptr<parser::FunctionStmt> functionStmt = program.get<parser::FunctionStmt>(0);
    EXPECT_EQ("foo", functionStmt->identifier);
    EXPECT_EQ(parser::StmtType::FUNCTION, functionStmt.get()->type);

    EXPECT_EQ(1, functionStmt->stmts.size());
}

TEST(ParserTest, ItShouldBeAbleToParserMainWithAPrint)
{
    std::string sourceCode = "function main() {print (\"Hello, World!\");};";

    std::deque<lexer::Token> tokens = lexer::lex(sourceCode);

    parser::Parser testObject(tokens);
    parser::Program program = testObject.parse();

    EXPECT_EQ(1, program.stmts.size());

    std::shared_ptr<parser::FunctionStmt> functionStmt = program.get<parser::FunctionStmt>(0);
    EXPECT_EQ("main", functionStmt->identifier);
    EXPECT_EQ(parser::StmtType::FUNCTION, functionStmt.get()->type);

    EXPECT_EQ(1, functionStmt->stmts.size());

    std::shared_ptr<parser::Stmt> printStmt = functionStmt->stmts[0];

    EXPECT_EQ(parser::StmtType::PRINT, printStmt->type);

    parser::PrintStmt *printStmtPointer = (parser::PrintStmt *)printStmt.get();
    parser::StringLiteral *stringLiteralPointer = (parser::StringLiteral *)printStmtPointer->expr.get();

    EXPECT_EQ(stringLiteralPointer->type, parser::ExprType::STRING_LITERAL);
    EXPECT_EQ("Hello, World!", stringLiteralPointer->literal);
}

TEST(ParserTest, ItShouldBeAbleToParserPrintWithEmptyStringBody)
{
    std::string sourceCode = "function main() {print (\"\");};";

    std::deque<lexer::Token> tokens = lexer::lex(sourceCode);

    parser::Parser testObject(tokens);
    parser::Program program = testObject.parse();

    EXPECT_EQ(1, program.stmts.size());

    std::shared_ptr<parser::FunctionStmt> functionStmt = program.get<parser::FunctionStmt>(0);
    EXPECT_EQ("main", functionStmt->identifier);
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
    function main() {
        print"Hello World!");
    };)";

    std::deque<lexer::Token> tokens = lexer::lex(sourceCode);

    parser::Parser testObject(tokens);
    parser::Program program = testObject.parse();

    EXPECT_EQ(1, program.stmts.size());
    EXPECT_FALSE(program.isSyntacticallyCorrect());

    std::vector<parser::ErrorLog> errors = program.errors;

    EXPECT_EQ(1, errors.size());

    parser::ErrorLog error = errors[0];
    EXPECT_EQ("Expected: LEFT_PAREN at line 3, column 15, but found \"\"Hello World!\"\".", error.getMessage());
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