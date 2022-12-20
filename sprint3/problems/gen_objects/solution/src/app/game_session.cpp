#include "game_session.h"
#include "random_generators.h"
#include "support_types.h"

namespace app {

const GameSession::Id& GameSession::GetId() const noexcept {
    return id_;
}

const std::shared_ptr<model::Map> GameSession::GetMap() {
    return map_;
};

std::shared_ptr<GameSession::SessionStrand> GameSession::GetStrand() {
    return strand_;
};

std::weak_ptr<model::Dog> GameSession::CreateDog(const std::string& dog_name, const model::Map& map, bool randomize_spawn_points){
    auto dog = std::make_shared<model::Dog>(dog_name);
    if(randomize_spawn_points) {
        LocateDogInRandomPositionOnMap(dog);
    } else {
        LocateDogInStartPointOnMap(dog);
    }
    dogs_[dog->GetId()] = dog;
    if(dogs_.size() == 1) {
        GenerateLoot(loot_generator_.GetPeriod());
        generate_loot_ticker_ = std::make_shared<time_m::Ticker>(
            strand_,
            loot_generator_.GetPeriod(),
            std::bind(&GameSession::GenerateLoot, this, std::placeholders::_1)
        );
        generate_loot_ticker_->Start();
    }
    return dog;
};

const GameSession::LostObjects& GameSession::GetLostObjects() {
    return lost_objects_;
};

void GameSession::UpdateGameState(const GameSession::TimeInterval& delta_time) {
    for(auto [dog_id, dog] : dogs_) {
        auto [new_position, new_velocity] = map_->GetValidMove(
            dog->GetPosition(),
            dog->CalculateNewPosition(delta_time),
            dog->GetVelocity()
        );
        dog->SetPosition(new_position);
        dog->SetVelocity(new_velocity);
    }
};

void GameSession::GenerateLoot(const GameSession::TimeInterval& delta_time) {
    auto num_new_lost_obj = loot_generator_.Generate(delta_time,
                                                    lost_objects_.size(),
                                                    dogs_.size());
    for(size_t i = 0; i < num_new_lost_obj; ++i) {
        CreateLostObject();
    }
};

void GameSession::CreateLostObject() {
    auto lost_obj = std::make_shared<model::LostObject>();
    SetRandomLootType(lost_obj);
    LocateLootInRandomPositionOnMap(lost_obj);
    lost_objects_[lost_obj->GetId()] = lost_obj;
};

void GameSession::SetRandomLootType(std::shared_ptr<model::LostObject> loot) {
    auto type = utils::GenerateSizeTFromInterval(0, map_->GetNumberOfLootTypes() - 1);
    loot->SetType(type);
};

void GameSession::LocateLootInRandomPositionOnMap(std::shared_ptr<model::LostObject> loot) {
    loot->SetPosition(map_->GenerateRandomPosition());
};

void GameSession::LocateDogInRandomPositionOnMap(std::shared_ptr<model::Dog> dog) {
    dog->SetPosition(map_->GenerateRandomPosition());
};

void GameSession::LocateDogInStartPointOnMap(std::shared_ptr<model::Dog> dog) {
    double x, y;
    auto roads = map_->GetRoads();
    auto road = roads[0];
    dog->SetPosition({static_cast<double>(road.GetStart().x),
                        static_cast<double>(road.GetStart().y)});
};

}