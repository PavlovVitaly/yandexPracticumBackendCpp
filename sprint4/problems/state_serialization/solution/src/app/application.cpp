#include "application.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/thread/future.hpp>
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
        game_session = std::make_shared<GameSession>(game_.FindMap(id), tick_period_, game_.GetLootGeneratorConfig(), ioc_);
        AddGameSession(game_session);
        game_session->Run();
    }
    auth_token_to_session_index_[token] = game_session;
    BoundPlayerAndGameSession(player, game_session);
    game_session_to_token_player_pair_[game_session][token] = player;
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
    auto dog = session->CreateDog(player->GetName(), *(session->GetMap()), randomize_spawn_points_);
    player->SetDog(dog);
};

const std::vector< std::shared_ptr<Player> >& Application::GetPlayersFromGameSession(const authentication::Token& token) {
    static const std::vector< std::shared_ptr<Player> > emptyPlayerList;
    auto player = player_tokens_.FindPlayerBy(token);
    auto session_id = player->GetGameSessionId();
    if(!session_id_to_players_.contains(session_id)) {
        return emptyPlayerList;
    }
    return session_id_to_players_[session_id];
};

bool Application::IsExistPlayer(const authentication::Token& token) {
    return static_cast<bool>(player_tokens_.FindPlayerBy(token));
};

void Application::SetPlayerAction(const authentication::Token& token, model::Direction direction) {
    auto player = player_tokens_.FindPlayerBy(token);
    auto dog = player->GetDog().lock();
    double velocity = player->GetGameSession()->GetMap()->GetDogVelocity();
    dog->SetAction(direction, velocity);
};

bool Application::IsManualTimeManagement() {
    return tick_period_.count() == 0;
};

void Application::UpdateGameState(const std::chrono::milliseconds& delta_time) {
    for(auto session : sessions_) {
        session->UpdateGameState(delta_time);
    }
    SaveGameState(delta_time);
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

std::shared_ptr<GameSession> Application::FindGameSessionBy(const authentication::Token& token) const noexcept {
    if (auto it = auth_token_to_session_index_.find(token); it != auth_token_to_session_index_.end()) {
        return it->second;
    }
    return nullptr;
};

const std::vector< std::shared_ptr<GameSession> >& Application::GetSessions() {
    return sessions_;
};

void Application::RestoreGameState(saving::SavingSettings saving_settings) {
    saving_settings_ = std::move(saving_settings);
    RestoreGame();
    if(!(saving_settings_.state_file_path
        && saving_settings_.period)) {
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

std::vector<game_data_ser::GameSessionSerialization> Application::GetSerializedData() {
    using game_data_ser::GameSessionSerialization;
    std::vector<GameSessionSerialization> sessions_ser;
    for(auto session_ptr : sessions_){
        boost::promise<GameSessionSerialization> promise;
        auto res_future = promise.get_future();
        net::dispatch(*(session_ptr->GetStrand()),
        [self = shared_from_this()
        , &promise
        , session_ptr] {
            promise.set_value(
                GameSessionSerialization(*session_ptr, self->game_session_to_token_player_pair_.at(session_ptr))
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
        auto game_session = std::make_shared<GameSession>(game_.FindMap(item.RestoreMapId())
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
            auth_token_to_session_index_[token] = game_session;
            game_session_to_token_player_pair_[game_session][token] = player;
            player_tokens_.AddTokenPlayerPair(token, player);
            auto session_id = game_session->GetId();
            session_id_to_players_[session_id].push_back(player);
        }
        AddGameSession(game_session);
        game_session->Run();
    }
};

}