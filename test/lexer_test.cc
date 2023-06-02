#include <gtest/gtest.h>
#include "src/lexer.hh"
#include <stdexcept>

// TEST(LexerTest, ReadInIntegerDecl) {
//     std::string source = "integer a = 5;";
//     Lexer testObject(source);

//     Token integerType = testObject.next();

//     Location start(0, 0);
//     Location end(0, 7);

//     std::string raw = "integer";
//     Token exp(TokenType::INTEGER_TYPE, raw, start, end);

//     ASSERT_TRUE(exp == integerType);
// }

TEST(LexerTest, ItShouldCreateWithRowInfo)
{
    lexer::Location testObject(3, 10);
    ASSERT_EQ(3, testObject.getRow());
}

TEST(LexerTest, ItShouldCreateWithColumnInfo)
{
    lexer::Location testObject(3, 10);
    ASSERT_EQ(10, testObject.getColumn());
}

TEST(LexerTest, ItShouldThrowIllegalArgumentExceptionIfRowNegative)
{
    std::string caught_exception_message = "";
    try
    {
        lexer::Location testObject(-1, 10);
    }
    catch (const std::invalid_argument &e)
    {
        caught_exception_message = e.what();
    }

    EXPECT_EQ("Cannot construct location with negative row number [-1].", caught_exception_message);
}

TEST(LexerTest, ItShouldThrowIllegalArgumentExceptionIfColumnNegative)
{
    std::string caught_exception_message = "";
    try
    {
        lexer::Location testObject(1, -10);
    }
    catch (const std::invalid_argument &e)
    {
        caught_exception_message = e.what();
    }

    EXPECT_EQ("Cannot construct location with negative column number [-10].", caught_exception_message);
}