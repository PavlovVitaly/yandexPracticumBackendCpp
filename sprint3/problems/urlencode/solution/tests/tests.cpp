#include <gtest/gtest.h>

#include "../src/urlencode.h"

using namespace std::literals;

TEST(UrlEncodeTestSuite, EmptyString) {
    EXPECT_TRUE(UrlEncode(""sv).empty());
}

TEST(UrlEncodeTestSuite, OrdinaryCharsAreNotEncoded) {
    const std::string orig{"valid-url_string"};
    const std::string expected{"valid-url_string"};
    EXPECT_EQ(UrlEncode(orig), expected);
}

TEST(UrlEncodeTestSuite, ComplexCharsAreEncoded) {
    const std::string orig{"$vAliD&compLex @URL sTRing? [Hello]"};
    const std::string expected{"%24vAliD%26compLex+%40URL+sTRing%3f+%5bHello%5d"};
    EXPECT_EQ(UrlEncode(orig), expected);
}

TEST(UrlEncodeTestSuite, StringWithSpaces) {
    const std::string orig{"valid url str with plus symbol"};
    const std::string expected{"valid+url+str+with+plus+symbol"};
    EXPECT_EQ(UrlEncode(orig), expected);
}
