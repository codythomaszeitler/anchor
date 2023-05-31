#include <gtest/gtest.h>
#include "src/lexer.hh"

TEST(LexerTest, ReadInIntegerDecl) {
    std::string source = "integer a = 5;";
    Lexer testObject(source);
}