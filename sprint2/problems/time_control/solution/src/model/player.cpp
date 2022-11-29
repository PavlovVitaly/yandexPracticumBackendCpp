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

std::weak_ptr<GameSession> Player::GetGameSession() {
    return session_;
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

void Player::MoveDog(const std::chrono::milliseconds& delta_time) {
    auto dog = dog_.lock();
    Position potentional_posotion = dog->CalculateNewPosition(delta_time);
    if(session_.lock()->GetMap()->IsValidPosition(potentional_posotion)) {
        dog->SetPosition(potentional_posotion);
    };
};

}