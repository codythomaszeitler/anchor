#include <gtest/gtest.h>
#include "src/lexer.hh"
#include <stdexcept>

TEST(LexerTest, ReadInIntegerDecl)
{
    std::string source = "val a = 535;";
    lexer::Lexer testObject(source);

    lexer::Token integerTypeToken = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::VAL,
                  "val",
                  lexer::Location(1, 1),
                  lexer::Location(1, 3)),
              integerTypeToken);
    EXPECT_TRUE(testObject.hasNext());

    lexer::Token identifier = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::IDENTIFIER,
                  "a",
                  lexer::Location(1, 5),
                  lexer::Location(1, 5)),
              identifier);

    EXPECT_TRUE(testObject.hasNext());
    lexer::Token equals = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::EQUALS,
                  "=",
                  lexer::Location(1, 7),
                  lexer::Location(1, 7)),
              equals);

    EXPECT_TRUE(testObject.hasNext());
    lexer::Token numberFive = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::INTEGER,
                  "535",
                  lexer::Location(1, 9),
                  lexer::Location(1, 11)),
              numberFive);
    EXPECT_TRUE(testObject.hasNext());
    lexer::Token semicolon = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::SEMICOLON,
                  ";",
                  lexer::Location(1, 12),
                  lexer::Location(1, 12)),
              semicolon);
    EXPECT_FALSE(testObject.hasNext());
}

TEST(LexerTest, ReadInAdditionOfTwoNumbers)
{
    std::string source = "3 + 5 + 6";
    lexer::Lexer testObject(source);

    lexer::Token three = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::INTEGER,
                  "3",
                  lexer::Location(1, 1),
                  lexer::Location(1, 1)),
              three);

    lexer::Token plus = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::PLUS_SIGN,
                  "+",
                  lexer::Location(1, 3),
                  lexer::Location(1, 3)),
              plus);
    lexer::Token two = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::INTEGER,
                  "5",
                  lexer::Location(1, 5),
                  lexer::Location(1, 5)),
              two);

    lexer::Token secondPlusSign = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::PLUS_SIGN,
                  "+",
                  lexer::Location(1, 7),
                  lexer::Location(1, 7)),
              secondPlusSign);

    lexer::Token six = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::INTEGER,
                  "6",
                  lexer::Location(1, 9),
                  lexer::Location(1, 9)),
              six);
}

TEST(LexerTest, ReadInVoidFunctionDeclaration)
{
    std::string source = "function foo() {};";
    lexer::Lexer testObject(source);

    lexer::Token functionToken = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::FUNCTION,
                  "function",
                  lexer::Location(1, 1),
                  lexer::Location(1, 8)),
              functionToken);

    lexer::Token fooToken = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::IDENTIFIER,
                  "foo",
                  lexer::Location(1, 10),
                  lexer::Location(1, 12)),
              fooToken);

    lexer::Token leftParen = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::LEFT_PAREN,
                  "(",
                  lexer::Location(1, 13),
                  lexer::Location(1, 13)),
              leftParen);
    lexer::Token rightParen = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::RIGHT_PAREN,
                  ")",
                  lexer::Location(1, 14),
                  lexer::Location(1, 14)),
              rightParen);

    lexer::Token leftBracket = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::LEFT_BRACKET,
                  "{",
                  lexer::Location(1, 16),
                  lexer::Location(1, 16)),
              leftBracket);

    lexer::Token rightBracket = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::RIGHT_BRACKET,
                  "}",
                  lexer::Location(1, 17),
                  lexer::Location(1, 17)),
              rightBracket);

    lexer::Token semicolon = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::SEMICOLON,
                  ";",
                  lexer::Location(1, 18),
                  lexer::Location(1, 18)),
              semicolon);
}

TEST(LexerTest, ReadInReturnStatementWithNumber)
{
    std::string source = "function foo() {return 5;};";
    lexer::Lexer testObject(source);

    lexer::Token functionToken = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::FUNCTION,
                  "function",
                  lexer::Location(1, 1),
                  lexer::Location(1, 8)),
              functionToken);

    lexer::Token fooToken = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::IDENTIFIER,
                  "foo",
                  lexer::Location(1, 10),
                  lexer::Location(1, 12)),
              fooToken);

    lexer::Token leftParen = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::LEFT_PAREN,
                  "(",
                  lexer::Location(1, 13),
                  lexer::Location(1, 13)),
              leftParen);
    lexer::Token rightParen = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::RIGHT_PAREN,
                  ")",
                  lexer::Location(1, 14),
                  lexer::Location(1, 14)),
              rightParen);

    lexer::Token leftBracket = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::LEFT_BRACKET,
                  "{",
                  lexer::Location(1, 16),
                  lexer::Location(1, 16)),
              leftBracket);

    lexer::Token returnToken = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::RETURN,
                  "return",
                  lexer::Location(1, 17),
                  lexer::Location(1, 22)),
              returnToken);

    lexer::Token five = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::INTEGER,
                  "5",
                  lexer::Location(1, 24),
                  lexer::Location(1, 24)),
              five);

    lexer::Token semicolon = testObject.next();
    EXPECT_EQ(lexer::Token(
                  lexer::TokenType::SEMICOLON,
                  ";",
                  lexer::Location(1, 25),
                  lexer::Location(1, 25)),
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

TEST(TokenTest, ItShouldBeAbleToStringifyATokenType) {
    lexer::TokenType equalsTokenType = lexer::TokenType::EQUALS;
    std::string asString = lexer::tostring(equalsTokenType);
    EXPECT_EQ("EQUALS", asString);
    EXPECT_EQ("SEMICOLON", lexer::tostring(lexer::TokenType::SEMICOLON));
}