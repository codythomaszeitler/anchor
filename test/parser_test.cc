#include <gtest/gtest.h>
#include "src/lexer.hh"
#include "src/parser.hh"

TEST(ParserTest, ItShouldParseFunctionDefinition) {
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