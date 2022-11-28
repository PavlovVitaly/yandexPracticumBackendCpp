#include "player.h"

namespace model {

const Player::Id& Player::GetId() const {
    return id_;
};

const std::string& Player::GetName() const {
    return name_;
};

const model::GameSession::Id& Player::GetGameSessionId() const {
    return session_.lock()->GetId();
};

void Player::SetGameSession(std::weak_ptr<GameSession> session) {
    session_ = session;
};

std::weak_ptr<Dog> Player::GetDog() {
    return dog_;
};

void Player::SetDog(std::weak_ptr<Dog> dog) {
    dog_ = dog;
};

}