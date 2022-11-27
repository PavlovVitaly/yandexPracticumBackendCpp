#include "player.h"

namespace model {

const Player::Id& Player::GetId() const {
    return id_;
};

const std::string& Player::GetName() const {
    return name_;
};

}