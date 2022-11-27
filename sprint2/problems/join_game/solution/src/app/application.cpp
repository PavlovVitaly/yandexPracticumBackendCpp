#include "application.h"

namespace app {

const model::Game::Maps& Application::ListMap() const noexcept {
    return game_.GetMaps();
};

const std::shared_ptr<model::Map> Application::FindMap(const model::Map::Id& id) const noexcept {
    return game_.FindMap(id);
};

authentication::Token Application::JoinGame(const model::Map::Id& id, const std::string& player_name) {
    auto player = CreatePlayer(player_name);
    return player_tokens_.AddPlayer(player);
};

std::shared_ptr<model::Player> Application::CreatePlayer(const std::string& player_name) {
    auto player = std::make_shared<model::Player>(player_name);
    players_.push_back(player);
    return player;
};

}