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

    EXPECT_EQ("Cannot construct location with non-positive row number [-1].", caught_exception_message);
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

    EXPECT_EQ("Cannot construct location with non-positive column number [-10].", caught_exception_message);
}

TEST(LexerTest, ItShouldBeAbleToUseCopyConstructor)
{
    lexer::Location source(1, 2);
    lexer::Location target(source);

    EXPECT_EQ(source.getRow(), target.getRow());
    EXPECT_EQ(source.getColumn(), target.getColumn());

    EXPECT_NE(&source, &target);
}

TEST(LexerTest, ItShouldBeAbleToDoAssignmentOperation)
{
    lexer::Location source(1, 2);
    lexer::Location target = source;

    EXPECT_EQ(source.getRow(), target.getRow());
    EXPECT_EQ(source.getColumn(), target.getColumn());

    EXPECT_NE(&source, &target);
}

TEST(TokenTest, ItShouldBeAbleToCreateToken)
{
    lexer::Location start(1, 1);
    lexer::Location end(1, 5);

    lexer::Token testObject(lexer::TokenType::INTEGER_TYPE, "integer", start, end);

    EXPECT_EQ(lexer::TokenType::INTEGER_TYPE, testObject.getTokenType());
    EXPECT_EQ("integer", testObject.getRaw());

    EXPECT_EQ(lexer::Location(1,1), testObject.getStart());
    EXPECT_NE(lexer::Location(1,2), testObject.getStart());

    EXPECT_EQ(lexer::Location(1, 5), testObject.getEnd());
}