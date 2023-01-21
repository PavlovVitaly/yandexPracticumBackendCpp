#include "token_generator.h"

#include <iomanip>
#include <sstream>

namespace authentication {

const size_t NUMBER_OF_DIGITS_IN_HALF_TOKEN = 16;

Token TokenGenerator::GenerateToken() {
    std::stringstream ss;
    ss << std::setw(NUMBER_OF_DIGITS_IN_HALF_TOKEN) << std::setfill('0') << std::hex << generator1_();
    ss << std::setw(NUMBER_OF_DIGITS_IN_HALF_TOKEN) << std::setfill('0') << std::hex << generator2_();
    Token token{ss.str()};
    return token;
};

}