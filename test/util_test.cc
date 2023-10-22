#include <gtest/gtest.h>
#include <vector>
#include "src/util.hh"

TEST(UtilTest, ItShouldBeAbleToJoinThreeStrings) {
    std::vector<std::string> strings{"a", "b", "c"};

    std::string joined = util::join(strings.begin(), strings.end(), ",");
    EXPECT_EQ("a,b,c", joined);
}

TEST(UtilTest, ItShouldJoinToNothing) {
    std::vector<std::string> strings;

    std::string joined = util::join(strings.begin(), strings.end(), ",");
    EXPECT_EQ("", joined);
}