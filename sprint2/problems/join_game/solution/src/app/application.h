#pragma once
#include "game.h"
#include "player.h"
#include "player_tokens.h"

#include <vector>
#include <memory>
#include <tuple>

namespace app {

class Application {
public:
    Application(model::Game game) : game_(std::move(game)) {};
    Application(const Application& other) = delete;
    Application(Application&& other) = delete;
    Application& operator = (const Application& other) = delete;
    Application& operator = (Application&& other) = delete;
    virtual ~Application() = default;

    const model::Game::Maps& ListMap() const noexcept;
    const std::shared_ptr<model::Map> FindMap(const model::Map::Id& id) const noexcept;
    std::tuple<authentication::Token, model::Player::Id> JoinGame(const std::string& player_name, const model::Map::Id& id);
private:
    model::Game game_;
    std::vector< std::shared_ptr<model::Player> > players_;
    authentication::PlayerTokens player_tokens_;

    std::shared_ptr<model::Player> CreatePlayer(const std::string& player_name);
};

}