#include "game_session.h"
#include "random_generators.h"

namespace model {

const GameSession::Id& GameSession::GetId() const noexcept {
    return id_;
}

const std::shared_ptr<Map> GameSession::GetMap() {
    return map_;
};

std::shared_ptr<GameSession::SessionStrand> GameSession::GetStrand() {
    return strand_;
};

std::shared_ptr<Dog> GameSession::CreateDog(const std::string& dog_name){
    auto dog = std::make_shared<Dog>(dog_name);
    dogs_.push_back(dog);
    //LocateDogInRandomPositionOnMap(dog);  // todo: commented for testing. Later should be uncommented.
    return dog;
};

void GameSession::LocateDogInRandomPositionOnMap(std::shared_ptr<Dog> dog) {
    double x, y;
    auto roads = map_->GetRoads();
    int road_index = utils::GenerateIntegerFromInterval(0, roads.size() - 1);
    auto road = roads[road_index];
    if(road.IsHorizontal()) {
        x = utils::GenerateDoubleFromInterval(road.GetStart().x,
                                                road.GetEnd().x);
        y = road.GetStart().y;
    } else {
        y = utils::GenerateDoubleFromInterval(road.GetStart().y,
                                                road.GetEnd().y);
        x = road.GetStart().x;
    }
    dog->SetPosition({x, y});
};

}