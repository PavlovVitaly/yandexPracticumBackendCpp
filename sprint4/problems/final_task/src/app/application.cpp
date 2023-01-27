#include "application.h"
#include "database_invariants.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/thread/future.hpp>
#include <ranges>
#include <algorithm>

namespace app {

using namespace std::literals;

const model::Game::Maps& Application::ListMap() const noexcept {
    return game_.GetMaps();
};

std::optional<std::shared_ptr<const model::Map>> Application::FindMap(const model::Map::Id& id) const noexcept {
    return game_.FindMap(id);
};

std::tuple<authentication::Token, Player::Id> Application::JoinGame(
        const std::string& player_name,
        const model::Map::Id& id) {
    auto player = CreatePlayer(player_name);
    auto token = token_generator_.GenerateToken();
    auto game_session = FindGameSessionBy(id);
    if(!game_session){
        game_session = std::make_shared<GameSession>(
            *game_.FindMap(id),
            tick_period_,
            game_.GetLootGeneratorConfig(),
            ioc_);
        AddGameSession(*game_session);
        game_session.value()->Run();
    }
    BoundPlayerAndGameSession(player, *game_session);
    session_id_to_token_player_pairs_[game_session.value()->GetId()][token] = player->GetId();
    return std::tie(token, player->GetId());
};

std::shared_ptr<Player> Application::CreatePlayer(const std::string& player_name) {
    auto player = std::make_shared<Player>(player_name);
    players_[player->GetId()] = player;
    return player;
};

void Application::BoundPlayerAndGameSession(std::shared_ptr<Player> player,
                                    std::shared_ptr<GameSession> session){
    player->SetGameSession(session);
    auto dog = session->CreateDog(player->GetName(), *(session->GetMap()), randomize_spawn_points_);
    player->SetDog(dog);
};

std::vector< std::shared_ptr<Player>> Application::GetPlayersFromGameSession(
    const authentication::Token& token) {
    auto player = FindPlayerBy(token);
    if(!player) {
        return std::vector< std::shared_ptr<Player>>();
    }
    auto session_id = player.value()->GetGameSessionId();
    if(!session_id_to_token_player_pairs_.contains(session_id)) {
        return std::vector< std::shared_ptr<Player>>();
    }
    std::vector< std::shared_ptr<Player> > players;
    std::ranges::transform(
        session_id_to_token_player_pairs_.at(session_id),
        std::back_inserter(players),
        [self = shared_from_this()](const auto& token_to_player_id) {
            const auto& [token, player_id] = token_to_player_id;
            if(self->players_.contains(player_id)) {
                return self->players_[player_id];
            }
            return std::shared_ptr<Player>();
        }
    );
    return players;
};

bool Application::IsExistPlayer(const authentication::Token& token) {
    return static_cast<bool>(FindPlayerBy(token));
};

void Application::SetPlayerAction(const authentication::Token& token, model::Direction direction) {
    auto player = FindPlayerBy(token);
    if(!player) {
        return;
    }
    auto dog = player.value()->GetDog().lock();
    double velocity = player.value()
                    ->GetGameSession()
                    ->GetMap()
                    ->GetDogVelocity();
    dog->SetAction(direction, velocity);
};

bool Application::IsManualTimeManagement() {
    return tick_period_.count() == 0;
};

void Application::UpdateGameState(const std::chrono::milliseconds& delta_time) {
    for(auto item : sessions_) {
        auto [session_id, session] = item;
        boost::promise<void> res_promise;
        auto res_future = res_promise.get_future();
        net::dispatch(*(session->GetStrand()),
            [session
            , &delta_time
            , &res_promise] {
                session->UpdateGameState(delta_time);
                res_promise.set_value();
            }
        );
        res_future.get();
    }
    SaveGameState(delta_time);
};

void Application::AddGameSession(std::shared_ptr<GameSession> session) {
    if (auto [it, inserted] = map_id_to_session_index_.emplace(session->GetMap()->GetId(), session->GetId()); !inserted) {
        throw std::invalid_argument("Game session with map id "s + *(session->GetMap()->GetId()) + " already exists"s);
    } else {
        try {
            sessions_[session->GetId()] = session;
        } catch (...) {
            map_id_to_session_index_.erase(it);
            throw;
        }
    }
    session->AddHandlingFinishedPlayersEvent(
        [self = shared_from_this()](const std::vector<domain::PlayerRecord>& player_records) {
            self->CommitGameRecords(player_records);
        }
    );
    session->AddRemoveInactivePlayersHandler(
        [self = shared_from_this()](const GameSession::Id& session_id) {
            self->RemoveInactivePlayers(session_id);
        }
    );
};

std::optional<std::shared_ptr<GameSession>> Application::FindGameSessionBy(const model::Map::Id& id) const noexcept {
    if (auto it = map_id_to_session_index_.find(id); it != map_id_to_session_index_.end()) {
        return sessions_.at(it->second);
    }
    return std::nullopt;
};

std::optional<std::shared_ptr<GameSession>> Application::FindGameSessionBy(const authentication::Token& token) const noexcept {
    for(const auto& [session_id, token_to_player] : session_id_to_token_player_pairs_) {
        if(token_to_player.contains(token)) {
            return sessions_.at(session_id);
        }
    }
    return std::nullopt;
};

void Application::RestoreGameState(saving::SavingSettings saving_settings) {
    saving_settings_ = std::move(saving_settings);
    RestoreGame();
    if(!(saving_settings_.state_file_path
        && saving_settings_.period) || IsManualTimeManagement()) {
        return;
    }
    save_game_ticker_ = std::make_shared<time_m::Ticker>(
        ioc_,
        saving_settings_.period.value(),
        [self = shared_from_this()](const std::chrono::milliseconds& delta_time) {
            self->SaveGame();
        }
    );
    save_game_ticker_->Start();
};

void Application::SaveGameState(const std::chrono::milliseconds& delta_time) {
    static int period = saving_settings_.period
        ? saving_settings_.period.value().count() : 0;
    if(!saving_settings_.period) {
        return;
    }
    period -= delta_time.count();
    if(period <= 0) {
        SaveGame();
        period = saving_settings_.period.value().count();
    }
};

void Application::SaveGame() {
    using game_data_ser::GameSessionSerialization;
    if(!saving_settings_.state_file_path){
        return;
    }
    std::vector<GameSessionSerialization> sessions_ser = GetSerializedData();

    std::fstream output_fstream;
    output_fstream.open(saving_settings_.state_file_path.value(), std::ios_base::out);
    {
        boost::archive::text_oarchive oarchive{output_fstream};
        oarchive << sessions_ser;
    }
};

std::optional<std::vector<domain::PlayerRecord>> Application::GetRecordsTable(
    std::optional<size_t> offset, std::optional<size_t> limit) {
    size_t start{0};
    size_t records_limit{db_invariants::DEFAULT_LIMIT};
    if(offset) {
        start = *offset;
    }
    if(limit) {
        if(*limit > db_invariants::DEFAULT_LIMIT) {
            return std::nullopt;
        }
        records_limit = *limit;
    }
    return use_cases_.GetRecordsTable(start, records_limit);
};

std::vector<game_data_ser::GameSessionSerialization> Application::GetSerializedData() {
    using game_data_ser::GameSessionSerialization;
    std::vector<GameSessionSerialization> sessions_ser;
    for(auto& [session_id, session] : sessions_){
        boost::promise<GameSessionSerialization> promise;
        auto res_future = promise.get_future();
        net::dispatch(*(session->GetStrand()),
        [self = shared_from_this()
        , &promise
        , session] {
            std::unordered_map< authentication::Token,
                                std::shared_ptr<app::Player>,
                                authentication::TokenHasher > token_to_palyer;
            std::ranges::transform(
                self->session_id_to_token_player_pairs_.at(session->GetId()),
                std::inserter(token_to_palyer, token_to_palyer.end()),
                [self = self->shared_from_this()](const auto& token_to_player_id) {
                    const auto& [token, player_id] = token_to_player_id;
                    auto player = self->players_.at(player_id);
                    return std::make_pair(token, player);
                }
            );
            promise.set_value(
                GameSessionSerialization(*session, std::move(token_to_palyer))
            );
        });
        sessions_ser.push_back(std::move(res_future.get())); // todo: not parallel solution, need fix. 
    };
    return sessions_ser;
};

void Application::RestoreGame() {
    using game_data_ser::GameSessionSerialization;
    if(!saving_settings_.state_file_path){
        return;
    }
    std::vector<GameSessionSerialization> sessions_ser;

    std::fstream input_fstream;
    input_fstream.open(saving_settings_.state_file_path.value(), std::ios_base::in);
    if(!input_fstream.is_open()) {
        return;
    }
    
    boost::archive::text_iarchive iarchive{input_fstream};
    iarchive >> sessions_ser;
    input_fstream.close();
    
    for(auto& item : sessions_ser) {
        auto game_session = std::make_shared<GameSession>(*game_.FindMap(item.RestoreMapId())
                                                        , tick_period_
                                                        , game_.GetLootGeneratorConfig()
                                                        , ioc_);
        for(auto& lost_obj_ser : item.GetLostObjectsSerialize()) {
            game_session->AddLostObject(std::move(lost_obj_ser.Restore()));
        }
        for(auto& player_ser : item.GetPlayersSerialize()) {
            auto player = std::make_shared<app::Player>(std::move(player_ser.Restore()));
            auto dog = std::make_shared<model::Dog>(std::move(player_ser.RestoreDog()));
            game_session->AddDog(dog);
            player->SetDog(dog);
            player->SetGameSession(game_session);
            auto token = player_ser.RestoreToken();
            session_id_to_token_player_pairs_[game_session->GetId()][token] = player->GetId();
        }
        AddGameSession(game_session);
        game_session->Run();
    }
};

std::optional<std::shared_ptr<Player>> Application::FindPlayerBy(authentication::Token token) {
    for(const auto& [session_id, token_to_player] : session_id_to_token_player_pairs_) {
        if(token_to_player.contains(token)) {
            return players_.at(token_to_player.at(token));
        }
    }
    return std::nullopt;
};

void Application::CommitGameRecords(const std::vector<domain::PlayerRecord>& player_records) {
    use_cases_.AddPlayerRecords(player_records);
};

void Application::RemoveInactivePlayers(const GameSession::Id& session_id) {
    std::unordered_map< authentication::Token,
                        Player::Id,
                        authentication::TokenHasher > token_to_player_id_for_delete;
    
    std::ranges::copy(
        session_id_to_token_player_pairs_.at(session_id) 
        | std::ranges::views::filter(
            [self = shared_from_this()](const auto& item) {
                const auto& [token, player_id] = item;
                return self->players_.at(player_id)->GetDog().expired();
            }
        ), std::inserter(token_to_player_id_for_delete, token_to_player_id_for_delete.end())
    );

    std::erase_if(session_id_to_token_player_pairs_.at(session_id),
        [&token_to_player_id_for_delete](const auto& item) {
            const auto& [token, player_id] = item;
            return token_to_player_id_for_delete.contains(token);
    });

    for(const auto& [token, player_id] : token_to_player_id_for_delete) {
        players_.erase(player_id);
    }
};

}