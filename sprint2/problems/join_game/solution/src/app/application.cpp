#include "application.h"

#include <iostream>

namespace app {

const model::Game::Maps& Application::ListMap() const noexcept {
    return game_.GetMaps();
};

const std::shared_ptr<model::Map> Application::FindMap(const model::Map::Id& id) const noexcept {
    return game_.FindMap(id);
};

std::tuple<authentication::Token, model::Player::Id> Application::JoinGame(
        const std::string& player_name,
        const model::Map::Id& id) {
    auto player = CreatePlayer(player_name);
    auto token = player_tokens_.AddPlayer(player);
    std::shared_ptr<model::GameSession> game_session = game_.FindGameSessionBy(id);
    if(!game_session){
        game_session = std::make_shared<model::GameSession>(game_.FindMap(id), ioc_);
        game_.AddGameSession(game_session);
    }
    BoundPlayerAndGameSession(player, game_session);
    return std::tie(token, player->GetId());
};

std::shared_ptr<model::Player> Application::CreatePlayer(const std::string& player_name) {
    auto player = std::make_shared<model::Player>(player_name);
    players_.push_back(player);
    return player;
};

void Application::BoundPlayerAndGameSession(std::shared_ptr<model::Player> player,
                                    std::shared_ptr<model::GameSession> session){
    session_id_to_players_[session->GetId()].push_back(player);
    player->SetGameSession(session);
    player->SetDog(session->CreateDog(player->GetName()));
};

const std::vector< std::weak_ptr<model::Player> >& Application::GetPlayersFromGameSession(authentication::Token token) {
    static const std::vector< std::weak_ptr<model::Player> > emptyPlayerList;
    auto player = player_tokens_.FindPlayerBy(token).lock();
    auto session_id = player->GetGameSessionId();
    if(!session_id_to_players_.contains(session_id)) {
        return emptyPlayerList;
    }
    return session_id_to_players_[session_id];
};

}