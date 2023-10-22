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

    parser::PrintStmt* printStmtPointer = (parser::PrintStmt*) printStmt.get(); 
    parser::StringLiteral* stringLiteralPointer = (parser::StringLiteral*) printStmtPointer->expr.get();

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

    parser::PrintStmt* printStmtPointer = (parser::PrintStmt*) printStmt.get(); 
    parser::StringLiteral* stringLiteralPointer = (parser::StringLiteral*) printStmtPointer->expr.get();

    EXPECT_EQ(stringLiteralPointer->type, parser::ExprType::STRING_LITERAL);
    EXPECT_EQ("", stringLiteralPointer->literal);
}