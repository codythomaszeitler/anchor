#include <gtest/gtest.h>
#include "src/lexer.hh"
#include <stdexcept>

TEST(LexerTest, ReadInIntegerDecl)
{
    std::string source = "integer a = 535;";
    lexer::Lexer testObject(source);

    lexer::Token integerTypeToken = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::INTEGER_TYPE,
                  "integer",
                  lexer::Location(0, 0),
                  lexer::Location(0, 6)),
              integerTypeToken);

    lexer::Token identifier = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::IDENTIFIER,
                  "a",
                  lexer::Location(0, 8),
                  lexer::Location(0, 8)),
              identifier);

    lexer::Token equals = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::EQUALS,
                  "=",
                  lexer::Location(0, 10),
                  lexer::Location(0, 10)),
              equals);

    lexer::Token numberFive = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::INTEGER,
                  "535",
                  lexer::Location(0, 12),
                  lexer::Location(0, 14)),
              numberFive);
    lexer::Token semicolon = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::SEMICOLON,
                  ";",
                  lexer::Location(0, 15),
                  lexer::Location(0, 15)),
              semicolon);
}

TEST(LexerTest, ReadInVoidFunctionDeclaration)
{
    std::string source = "function void foo() {};";
    lexer::Lexer testObject(source);

    lexer::Token functionToken = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::FUNCTION,
                  "function",
                  lexer::Location(0, 0),
                  lexer::Location(0, 7)),
              functionToken);

    lexer::Token identifierToken = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::IDENTIFIER,
                  "void",
                  lexer::Location(0, 9),
                  lexer::Location(0, 12)),
              identifierToken);

    lexer::Token fooToken = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::IDENTIFIER,
                  "foo",
                  lexer::Location(0, 14),
                  lexer::Location(0, 16)),
              fooToken);

    lexer::Token leftParen = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::LEFT_PAREN,
                  "(",
                  lexer::Location(0, 17),
                  lexer::Location(0, 17)),
              leftParen);
    lexer::Token rightParen = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::RIGHT_PAREN,
                  ")",
                  lexer::Location(0, 18),
                  lexer::Location(0, 18)),
              rightParen);

    lexer::Token leftBracket = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::LEFT_BRACKET,
                  "{",
                  lexer::Location(0, 20),
                  lexer::Location(0, 20)),
              leftBracket);

    lexer::Token rightBracket = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::RIGHT_BRACKET,
                  "}",
                  lexer::Location(0, 21),
                  lexer::Location(0, 21)),
              rightBracket);

    lexer::Token semicolon = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::SEMICOLON,
                  ";",
                  lexer::Location(0, 22),
                  lexer::Location(0, 22)),
              semicolon);
}

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

    EXPECT_EQ(lexer::Location(1, 1), testObject.getStart());
    EXPECT_NE(lexer::Location(1, 2), testObject.getStart());

    EXPECT_EQ(lexer::Location(1, 5), testObject.getEnd());
}