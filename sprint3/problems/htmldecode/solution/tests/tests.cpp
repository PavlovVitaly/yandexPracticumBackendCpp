#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

#include "../src/htmldecode.h"

namespace {
    const std::string TAG = "[HtmlDecode]";
}

using namespace std::literals;

TEST_CASE("Text without mnemonics", TAG) {
    const std::string orig{"hello"};
    const std::string expected{"hello"};
    CHECK(HtmlDecode(orig) == expected);
}

TEST_CASE("Text with mnemonics", TAG) {
    const std::string orig{"Johnson&amp;Johnson &lt;M&amp;M&apos;s&gt;"};
    const std::string expected{"Johnson&Johnson <M&M's>"};
    CHECK(HtmlDecode(orig) == expected);
}

TEST_CASE("Empty text", TAG) {
    CHECK(HtmlDecode(""sv).empty());
}

TEST_CASE("Text with capital letters mnemonics", TAG) {
    const std::string orig{"Johnson&AMP;Johnson &LT;M&AMP;M&APOS;s&GT;"};
    const std::string expected{"Johnson&Johnson <M&M's>"};
    CHECK(HtmlDecode(orig) == expected);
}

TEST_CASE("Text with mixed mnemonics", TAG) {
    const std::string orig{"Johnson&AMP;Johnson &lt;M&AMP;M&APOS;s&gt;"};
    const std::string expected{"Johnson&Johnson <M&M's>"};
    CHECK(HtmlDecode(orig) == expected);
}

TEST_CASE("Text with begin middle end mnemonics", TAG) {
    const std::string orig{"&lt;Johnson&amp;Johnson M&amp;M&apos;s&gt;"};
    const std::string expected{"<Johnson&Johnson M&M's>"};
    CHECK(HtmlDecode(orig) == expected);
}

TEST_CASE("Text with incomplete mnemonics", TAG) {
    const std::string orig{"&lt;Johnson&amp;Johnson M&am;M&apo;s&gt;"};
    const std::string expected{"<Johnson&Johnson M&am;M&apo;s>"};
    CHECK(HtmlDecode(orig) == expected);
}

TEST_CASE("Text with incomplete mnemonics with capital letters", TAG) {
    const std::string orig{"&lt;Johnson&amp;Johnson M&am;M&APO;s&gt;"};
    const std::string expected{"<Johnson&Johnson M&am;M&APO;s>"};
    CHECK(HtmlDecode(orig) == expected);
}

TEST_CASE("Text with mnemonics without ; symbol", TAG) {
    const std::string orig{"&lt;Johnson&ampJohnson M&ampM&apos;s&gt;"};
    const std::string expected{"<Johnson&Johnson M&M's>"};
    CHECK(HtmlDecode(orig) == expected);
}

TEST_CASE("Text with invalid mnemonics", TAG) {
    const std::string orig{"&lT;Johnson&amp;Johnson M&aMp;M&apos;s&gt;"};
    const std::string expected{"&lT;Johnson&Johnson M&aMp;M's>"};
    CHECK(HtmlDecode(orig) == expected);
}
