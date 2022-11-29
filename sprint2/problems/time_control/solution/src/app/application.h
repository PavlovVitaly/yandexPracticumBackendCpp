#pragma once
#include "game.h"
#include "player.h"
#include "player_tokens.h"
#include "tagged.h"

#include <vector>
#include <memory>
#include <tuple>
#include <unordered_map>

namespace app {

namespace net = boost::asio;

class Application {
public:
    using AppStrand = net::strand<net::io_context::executor_type>;

    Application(model::Game game, net::io_context& ioc) :
        game_(std::move(game)),
        ioc_(ioc),
        strand_(std::make_shared<AppStrand>(net::make_strand(ioc))) {};
    Application(const Application& other) = delete;
    Application(Application&& other) = delete;
    Application& operator = (const Application& other) = delete;
    Application& operator = (Application&& other) = delete;
    virtual ~Application() = default;

    const model::Game::Maps& ListMap() const noexcept;
    const std::shared_ptr<model::Map> FindMap(const model::Map::Id& id) const noexcept;
    std::tuple<authentication::Token, model::Player::Id> JoinGame(const std::string& player_name, const model::Map::Id& id);
    const std::vector< std::weak_ptr<model::Player> >& GetPlayersFromGameSession(const authentication::Token& token);
    bool IsExistPlayer(const authentication::Token& token);
    void SetPlayerAction(const authentication::Token& token, model::Direction direction);
    std::shared_ptr<AppStrand> GetStrand();

    void UpdateGameState(const std::chrono::milliseconds& delta_time);
private:
    using GameSessionIdHasher = util::TaggedHasher<model::GameSession::Id>;
    using GameSessionIdToIndex = std::unordered_map<model::GameSession::Id,
                                                    std::vector< std::weak_ptr<model::Player> >,
                                                    GameSessionIdHasher>;

    model::Game game_;
    std::vector< std::shared_ptr<model::Player> > players_;
    GameSessionIdToIndex session_id_to_players_;
    authentication::PlayerTokens player_tokens_;
    net::io_context& ioc_;
    std::shared_ptr<AppStrand> strand_; // todo: temp implementation.

    std::shared_ptr<model::Player> CreatePlayer(const std::string& player_name);
    void BoundPlayerAndGameSession(std::shared_ptr<model::Player> player,
                                    std::shared_ptr<model::GameSession> session);

};

}