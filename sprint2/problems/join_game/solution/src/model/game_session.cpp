#include "game_session.h"

namespace model {

const std::shared_ptr<Map> GameSession::GetMap() {
    return map_;
};

std::shared_ptr<GameSession::SessionStrand> GameSession::GetStrand() {
    return strand_;
};

std::shared_ptr<Dog> GameSession::CreateDog(const std::string& dog_name){
    auto dog = std::make_shared<Dog>(dog_name);
    dogs_.push_back(dog);
    return dog;
};

}