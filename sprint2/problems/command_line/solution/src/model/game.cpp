#include "game.h"

#include <cmath>
#include <string>

namespace model {

using namespace std::literals;

void Game::AddMap(const Map& map) {
    const size_t index = maps_.size();
    if (auto [it, inserted] = map_id_to_index_.emplace(map.GetId(), index); !inserted) {
        throw std::invalid_argument("Map with id "s + *map.GetId() + " already exists"s);
    } else {
        try {
            auto current_map = std::make_shared<Map>(map);
            if(!(std::abs(default_dog_velocity_ - INITIAL_DOG_VELOCITY) < EPSILON) &&
                std::abs(current_map->GetDogVelocity() - INITIAL_DOG_VELOCITY) < EPSILON) {
                current_map->SetDogVelocity(default_dog_velocity_);
            }
            maps_.push_back(current_map);
        } catch (...) {
            map_id_to_index_.erase(it);
            throw;
        }
    }
}

void Game::AddMaps(const std::vector<Map>& maps){
    for(auto item : maps){
        AddMap(item);
    }
};

const Game::Maps& Game::GetMaps() const noexcept {
    return maps_;
}

const std::shared_ptr<Map> Game::FindMap(const Map::Id& id) const noexcept {
    if (auto it = map_id_to_index_.find(id); it != map_id_to_index_.end()) {
        return maps_.at(it->second);
    }
    return nullptr;
};

void Game::SetDefaultDogVelocity(double velocity) {
        default_dog_velocity_ = std::abs(velocity);
        if(std::abs(default_dog_velocity_ - INITIAL_DOG_VELOCITY) < EPSILON) {
            return;
        }
        for(auto map : maps_) {
            if(std::abs(map->GetDogVelocity() - INITIAL_DOG_VELOCITY) < EPSILON) {
                map->SetDogVelocity(default_dog_velocity_);
            }
        }
    };

double Game::GetDefaultDogVelocity() const noexcept {
    return default_dog_velocity_;
};

}