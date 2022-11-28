#include "game.h"

#include <string>

namespace model {

using namespace std::literals;

void Game::AddMap(Map map) {
    const size_t index = maps_.size();
    if (auto [it, inserted] = map_id_to_index_.emplace(map.GetId(), index); !inserted) {
        throw std::invalid_argument("Map with id "s + *map.GetId() + " already exists"s);
    } else {
        try {
            maps_.push_back(std::make_shared<Map>(std::move(map)));
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

void Game::AddGameSession(std::shared_ptr<GameSession> session) {
    const size_t index = sessions_.size();
    if (auto [it, inserted] = map_id_to_session_index_.emplace(session->GetMap()->GetId(), index); !inserted) {
        throw std::invalid_argument("Game session with map id "s + *(session->GetMap()->GetId()) + " already exists"s);
    } else {
        try {
            sessions_.push_back(session);
        } catch (...) {
            map_id_to_session_index_.erase(it);
            throw;
        }
    }
};

std::shared_ptr<GameSession> Game::FindGameSessionBy(const Map::Id& id) const noexcept {
    if (auto it = map_id_to_session_index_.find(id); it != map_id_to_session_index_.end()) {
        return sessions_.at(it->second);
    }
    return nullptr;
};

}