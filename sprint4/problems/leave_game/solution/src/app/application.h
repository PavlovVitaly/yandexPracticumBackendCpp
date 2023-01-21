#pragma once
#include "game.h"
#include "player.h"
#include "player_tokens.h"
#include "tagged.h"
#include "saving_settings.h"
#include "game_session_serialization.h"
#include "ticker.h"

#include <vector>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <unordered_set>

namespace app {

namespace net = boost::asio;

class Application  : public std::enable_shared_from_this<Application>{
public:
    using AppStrand = net::strand<net::io_context::executor_type>;

    Application(model::Game game, size_t tick_period, bool randomize_spawn_points, net::io_context& ioc) :
            game_{std::move(game)},
            tick_period_{tick_period},
            randomize_spawn_points_{randomize_spawn_points},
            ioc_{ioc} {
    };
    Application(const Application& other) = delete;
    Application(Application&& other) = delete;
    Application& operator = (const Application& other) = delete;
    Application& operator = (Application&& other) = delete;
    virtual ~Application() = default;

    const model::Game::Maps& ListMap() const noexcept;
    const std::shared_ptr<model::Map> FindMap(const model::Map::Id& id) const noexcept;
    std::tuple<authentication::Token, Player::Id> JoinGame(const std::string& player_name, const model::Map::Id& id);
    std::vector< std::shared_ptr<Player> > GetPlayersFromGameSession(const authentication::Token& token);
    bool IsExistPlayer(const authentication::Token& token);
    void SetPlayerAction(const authentication::Token& token, model::Direction direction);
    bool IsManualTimeManagement();
    void UpdateGameState(const std::chrono::milliseconds& delta_time);
    void AddGameSession(std::shared_ptr<GameSession> session);
    std::shared_ptr<GameSession> FindGameSessionBy(const model::Map::Id& id) const noexcept;
    std::shared_ptr<GameSession> FindGameSessionBy(const authentication::Token& token) const noexcept;
    const std::vector< std::shared_ptr<app::GameSession> >& GetSessions();
    void RestoreGameState(saving::SavingSettings saving_settings);
    void SaveGame();
private:
    using PlayerIdHasher = util::TaggedHasher<Player::Id>;
    using Players = std::unordered_map<Player::Id,
                                    std::shared_ptr<Player>,
                                    PlayerIdHasher>;
    
    using MapIdHasher = util::TaggedHasher<model::Map::Id>;
    using MapIdToSessionIndex = std::unordered_map<model::Map::Id, size_t, MapIdHasher>;
    using AuthTokenToSessionIndex = std::unordered_map<authentication::Token, std::shared_ptr<GameSession>,
                                                        authentication::TokenHasher>;


    using GameSessionIdHasher = util::TaggedHasher<GameSession::Id>;

    using GameSessionIdToTokenPlayerPairs = std::unordered_map<GameSession::Id,
                                                    std::unordered_map< authentication::Token,
                                                                        Player::Id,
                                                                        authentication::TokenHasher >,
                                                    GameSessionIdHasher>;

    Players players_;
    GameSessionIdToTokenPlayerPairs session_id_to_token_player_pairs_;

    authentication::TokenGenerator token_generator_;

    std::vector< std::shared_ptr<app::GameSession> > sessions_;
    MapIdToSessionIndex map_id_to_session_index_;
    AuthTokenToSessionIndex auth_token_to_session_index_;

    model::Game game_;
    std::chrono::milliseconds tick_period_;
    bool randomize_spawn_points_;
    net::io_context& ioc_;
    saving::SavingSettings saving_settings_;
    std::shared_ptr<time_m::Ticker> save_game_ticker_;

    std::shared_ptr<Player> CreatePlayer(const std::string& player_name);
    void BoundPlayerAndGameSession(std::shared_ptr<Player> player,
                                    std::shared_ptr<GameSession> session);
    void SaveGameState(const std::chrono::milliseconds& delta_time);
    std::vector<game_data_ser::GameSessionSerialization> GetSerializedData();
    void RestoreGame();

    std::shared_ptr<app::Player> FindPlayerBy(authentication::Token token);
    void RemoveInactivePlayers(GameSession::Id session_id);
};

}