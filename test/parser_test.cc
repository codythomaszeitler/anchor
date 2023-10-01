#include <gtest/gtest.h>
#include "src/lexer.hh"
#include "src/parser.hh"

TEST(ParserTest, ItShouldParseFunctionDefinition) {
    std::string sourceCode = "function foo() {return 3 + 5;};";

    std::deque<lexer::Token> tokens = lexer::lex(sourceCode);

    parser::Parser testObject(tokens);

    std::vector<std::unique_ptr<parser::Stmt>> stmts = testObject.parse();

    EXPECT_EQ(1, stmts.size());
}