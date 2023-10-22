#include <gtest/gtest.h>
#include <vector>
#include "src/util.hh"

TEST(UtilTest, ItShouldBeAbleToJoinThreeStrings)
{
    std::vector<std::string> strings{"a", "b", "c"};

    std::string joined = util::join(strings.begin(), strings.end(), ",");
    EXPECT_EQ("a,b,c", joined);
}

TEST(UtilTest, ItShouldJoinToNothing)
{
    std::vector<std::string> strings;

    std::string joined = util::join(strings.begin(), strings.end(), ",");
    EXPECT_EQ("", joined);
}

TEST(UtilTest, ItShouldJoinJustASingleString) {
    std::vector<std::string> strings{"a"};

    std::string joined = util::join(strings.begin(), strings.end(), ",");
    EXPECT_EQ("a", joined);
}

TEST(UtilTest, ItShouldThrowIllegalArgumentExceptionIfEndIsBeforeBegin)
{
    std::vector<std::string> strings{"a", "b", "c"};

    try
    {
        util::join(strings.end(), strings.begin(), ",");
    }
    catch (std::invalid_argument &e)
    {
        EXPECT_STREQ("Begin iterator was after end iterator. No range found.", e.what());
    } 
    catch (...) {
        FAIL() << "Expected std::invalid_argument to have been thrown.";
    }
}