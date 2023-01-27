#include "game.h"
#include "model_invariants.h"

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
            if(default_dog_velocity_ &&
                std::abs(current_map->GetDogVelocity() - INITIAL_DOG_VELOCITY) < EPSILON) {
                current_map->SetDogVelocity(default_dog_velocity_.value());
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

std::optional<std::shared_ptr<const Map>> Game::FindMap(const Map::Id& id) const noexcept {
    if (auto it = map_id_to_index_.find(id); it != map_id_to_index_.end()) {
        return maps_.at(it->second);
    }
    return std::nullopt;
};

void Game::SetDefaultDogVelocity(double velocity) {
    default_dog_velocity_ = std::abs(velocity);
    for(auto map : maps_) {
        if(std::abs(map->GetDogVelocity() - INITIAL_DOG_VELOCITY) < EPSILON) {
            map->SetDogVelocity(default_dog_velocity_.value());
        }
    }
};

double Game::GetDefaultDogVelocity() const noexcept {
    return default_dog_velocity_ ? default_dog_velocity_.value() : INITIAL_DOG_VELOCITY;
};

void Game::AddLootGeneratorConfig(LootGeneratorConfig cfg) {
    loot_gen_cfg_ = std::move(cfg);
};

const LootGeneratorConfig& Game::GetLootGeneratorConfig() {
    return loot_gen_cfg_;
};

void Game::SetDefaultBagCapacity(size_t default_bag_capacity) {
    default_bag_capacity_ = default_bag_capacity;
    for(auto map : maps_) {
        if(map->GetBagCapacity() == INITIAL_BAG_CAPACITY) {
            map->SetBagCapacity(default_bag_capacity_.value());
        }
    }
};

size_t Game::GetDefaultBagCapacity() const noexcept {
    return default_bag_capacity_ ? default_bag_capacity_.value() : INITIAL_BAG_CAPACITY;
};

}