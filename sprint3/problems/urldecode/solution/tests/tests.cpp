#define BOOST_TEST_MODULE urlencode tests
#include <boost/test/unit_test.hpp>

#include "../src/urldecode.h"

BOOST_AUTO_TEST_CASE(UrlDecode_empty_string_tests) {
    using namespace std::literals;    
    BOOST_TEST(UrlDecode(""sv).empty());
}

BOOST_AUTO_TEST_CASE(UrlDecode_simple_string_tests) {
    const std::string orig{"valid-url_string"};
    const std::string expected{"valid-url_string"};
    BOOST_REQUIRE_EQUAL(UrlDecode(orig), expected);
}

BOOST_AUTO_TEST_CASE(UrlDecode_complex_string_tests) {
    const std::string orig{"%24vAliD%26compLex %40URL sTRing%3f+%5bHello%5d"};
    const std::string expected{"$vAliD&compLex @URL sTRing? [Hello]"};
    BOOST_REQUIRE_EQUAL(UrlDecode(orig), expected);
}

BOOST_AUTO_TEST_CASE(UrlDecode_invalid_string_tests) {
    const std::string invalid_simple_url_str{"valid-url%2gstring"};
    BOOST_REQUIRE_THROW(UrlDecode(invalid_simple_url_str), std::invalid_argument );
}

BOOST_AUTO_TEST_CASE(UrlDecode_string_with_plus_symbol_tests) {
    const std::string orig{"valid+url+str+with+plus+symbol"};
    const std::string expected{"valid url str with plus symbol"};
    BOOST_REQUIRE_EQUAL(UrlDecode(orig), expected);
}
