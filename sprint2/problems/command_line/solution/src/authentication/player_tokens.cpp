#include "player_tokens.h"

#include <iomanip>
#include <sstream>

namespace authentication {

const size_t NUMBER_OF_DIGITS_IN_HALF_TOKEN = 16;

Token PlayerTokens::AddPlayer(std::weak_ptr<app::Player> player) {
    std::stringstream ss;
    ss << std::setw(NUMBER_OF_DIGITS_IN_HALF_TOKEN) << std::setfill('0') << std::hex << generator1_();
    ss << std::setw(NUMBER_OF_DIGITS_IN_HALF_TOKEN) << std::setfill('0') << std::hex << generator2_();
    Token token{ss.str()};
    tokenToPalyer_[token] = player;
    return token;
};

std::weak_ptr<app::Player> PlayerTokens::FindPlayerBy(Token token) {
    if(!tokenToPalyer_.contains(token)){
        return std::weak_ptr<app::Player>();
    }
    return tokenToPalyer_[token];
};

}