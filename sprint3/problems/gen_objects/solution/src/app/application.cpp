#include "application.h"

#include <iostream>

namespace app {

using namespace std::literals;

const model::Game::Maps& Application::ListMap() const noexcept {
    return game_.GetMaps();
};

const std::shared_ptr<model::Map> Application::FindMap(const model::Map::Id& id) const noexcept {
    return game_.FindMap(id);
};

std::tuple<authentication::Token, Player::Id> Application::JoinGame(
        const std::string& player_name,
        const model::Map::Id& id) {
    auto player = CreatePlayer(player_name);
    auto token = player_tokens_.AddPlayer(player);
    std::shared_ptr<GameSession> game_session = FindGameSessionBy(id);
    if(!game_session){
        game_session = std::make_shared<GameSession>(game_.FindMap(id), ioc_);
        AddGameSession(game_session);
    }
    BoundPlayerAndGameSession(player, game_session);
    return std::tie(token, player->GetId());
};

std::shared_ptr<Player> Application::CreatePlayer(const std::string& player_name) {
    auto player = std::make_shared<Player>(player_name);
    players_.push_back(player);
    return player;
};

void Application::BoundPlayerAndGameSession(std::shared_ptr<Player> player,
                                    std::shared_ptr<GameSession> session){
    session_id_to_players_[session->GetId()].push_back(player);
    player->SetGameSession(session);
    player->CreateDog(player->GetName(), *(session->GetMap()), randomize_spawn_points_);
};

const std::vector< std::weak_ptr<Player> >& Application::GetPlayersFromGameSession(const authentication::Token& token) {
    static const std::vector< std::weak_ptr<Player> > emptyPlayerList;
    auto player = player_tokens_.FindPlayerBy(token).lock();
    auto session_id = player->GetGameSessionId();
    if(!session_id_to_players_.contains(session_id)) {
        return emptyPlayerList;
    }
    return session_id_to_players_[session_id];
};

bool Application::IsExistPlayer(const authentication::Token& token) {
    return !player_tokens_.FindPlayerBy(token).expired();
};

void Application::SetPlayerAction(const authentication::Token& token, model::Direction direction) {
    auto player = player_tokens_.FindPlayerBy(token).lock();
    auto dog = player->GetDog();
    double velocity = player->GetGameSession()->GetMap()->GetDogVelocity();
    dog->SetAction(direction, velocity);
};

bool Application::IsManualTimeManagement() {
    return tick_period_.count() == 0;
};

void Application::UpdateGameState(const std::chrono::milliseconds& delta_time) {
    for(auto player : players_) {
        player->MoveDog(delta_time);
    }
};

std::shared_ptr<Application::AppStrand> Application::GetStrand() {
    return strand_;
};

void Application::AddGameSession(std::shared_ptr<GameSession> session) {
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

std::shared_ptr<GameSession> Application::FindGameSessionBy(const model::Map::Id& id) const noexcept {
    if (auto it = map_id_to_session_index_.find(id); it != map_id_to_session_index_.end()) {
        return sessions_.at(it->second);
    }
    return nullptr;
};

}