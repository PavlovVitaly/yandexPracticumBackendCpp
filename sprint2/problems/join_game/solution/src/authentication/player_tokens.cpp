#include "player_tokens.h"

#include <sstream>

namespace authentication {

Token PlayerTokens::AddPlayer(std::weak_ptr<model::Player> player) {
    std::stringstream ss;
    ss << std::hex << generator1_();
    ss << std::hex << generator2_();
    return Token{ss.str()};
};

std::weak_ptr<model::Player> PlayerTokens::FindPlayerBy(Token token) {
    if(!tokenToPalyer_.contains(token)){
        return std::weak_ptr<model::Player>();
    }
    return tokenToPalyer_[token];
};

}