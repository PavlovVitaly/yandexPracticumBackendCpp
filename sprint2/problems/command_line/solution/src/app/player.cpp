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

std::shared_ptr<model::Dog> Player::GetDog() {
    return dog_;
};

void Player::MoveDog(const std::chrono::milliseconds& delta_time) {
    auto [new_position, new_velocity] = session_->GetMap()->GetValidMove(
                dog_->GetPosition(),
                dog_->CalculateNewPosition(delta_time),
                dog_->GetVelocity());
    dog_->SetPosition(new_position);
    dog_->SetVelocity(new_velocity);
};

void Player::CreateDog(const std::string& dog_name, const model::Map& map, bool randomize_spawn_points){
    dog_ = std::make_shared<model::Dog>(dog_name);
    if(randomize_spawn_points) {
        LocateDogInRandomPositionOnMap(map);
    } else {
        LocateDogInStartPointOnMap(map);
    }
};

void Player::LocateDogInRandomPositionOnMap(const model::Map& map) {
    double x, y;
    auto roads = map.GetRoads();
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
    dog_->SetPosition({x, y});
};

void Player::LocateDogInStartPointOnMap(const model::Map& map) {
    double x, y;
    auto roads = map.GetRoads();
    auto road = roads[0];
    dog_->SetPosition({static_cast<double>(road.GetStart().x),
                        static_cast<double>(road.GetStart().y)});
};

}