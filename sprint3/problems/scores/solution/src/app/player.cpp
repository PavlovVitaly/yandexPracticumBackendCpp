#include "player.h"
#include "random_generators.h"

namespace app {

const Player::Id& Player::GetId() const {
    return id_;
};

const std::string& Player::GetName() const {
    return name_;
};

const GameSession::Id& Player::GetGameSessionId() const {
    return session_->GetId();
};

std::shared_ptr<GameSession> Player::GetGameSession() {
    return session_;
};

void Player::SetGameSession(std::shared_ptr<GameSession> session) {
    session_ = session;
};

std::weak_ptr<model::Dog> Player::GetDog() {
    return dog_;
};

void Player::SetDog(std::weak_ptr<model::Dog> dog) {
    dog_ = dog;
};

}