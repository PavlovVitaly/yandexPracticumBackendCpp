#pragma once
#include "map.h"
#include "dog.h"
#include "player_record.h"
#include "tagged.h"
#include "loot_generator_config.h"
#include "loot_generator.h"
#include "lost_object.h"
#include "ticker.h"
#include "model_invariants.h"
#include "item_dog_provider.h"

#include <chrono>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/signals2/signal.hpp>
#include <functional>

namespace app {

namespace net = boost::asio;

class GameSession : public std::enable_shared_from_this<GameSession>  {
public:
    using SessionStrand = net::strand<net::io_context::executor_type>;
    using Id = util::Tagged<std::string, GameSession>;
    using TimeInterval = std::chrono::milliseconds;
    using LostObjectIdHasher = util::TaggedHasher<model::LostObject::Id>;
    using LostObjects = std::unordered_map<model::LostObject::Id,
                                            std::shared_ptr<model::LostObject>,
                                            LostObjectIdHasher>;
    using DogIdHasher = util::TaggedHasher<model::Dog::Id>;
    using Dogs = std::unordered_map<model::Dog::Id,
                                    std::shared_ptr<model::Dog>,
                                    DogIdHasher>;

    GameSession(std::shared_ptr<const model::Map> map,
                    const TimeInterval& period_of_update_game_state,
                    const model::LootGeneratorConfig& loot_gen_cfg,
                    net::io_context& ioc) :
            map_(map),
            ioc_(ioc),
            strand_(std::make_shared<SessionStrand>(net::make_strand(ioc_))),
            id_(*(map->GetId())),
            loot_generator_(
                TimeInterval(static_cast<uint64_t>(
                    loot_gen_cfg.period * model::MILLISECONDS_IN_SECOND)
                ),
                loot_gen_cfg.probability),
            period_of_update_game_state_(period_of_update_game_state) {
    };
    GameSession(GameSession&& other) = default;
    void Run();
    
    const Id& GetId() const noexcept;
    const std::shared_ptr<const model::Map> GetMap();
    std::shared_ptr<SessionStrand> GetStrand();
    std::weak_ptr<model::Dog> CreateDog(const std::string& dog_name,
                                        const model::Map& map,
                                        bool randomize_spawn_points);
    void UpdateGameState(const TimeInterval& delta_time);
    const LostObjects& GetLostObjects();
    void AddLostObject(model::LostObject lost_object);
    void AddDog(std::shared_ptr<model::Dog> dog);
    
    void AddRemoveInactivePlayersHandler(
        std::function<void(const GameSession::Id&)> handler);
    void AddHandlingFinishedPlayersEvent(
        std::function<void(const std::vector<domain::PlayerRecord>&)> handler);
    
private:
    std::shared_ptr<const model::Map> map_;
    net::io_context& ioc_;
    std::shared_ptr<SessionStrand> strand_;
    Id id_;
    loot_gen::LootGenerator loot_generator_;
    Dogs dogs_;
    LostObjects lost_objects_;
    TimeInterval period_of_update_game_state_;
    std::shared_ptr<time_m::Ticker> update_game_state_ticker_;
    std::shared_ptr<time_m::Ticker> generate_loot_ticker_;

    boost::signals2::signal<void (const GameSession::Id&)> remove_inactive_players_sig;
    boost::signals2::signal<void (const std::vector<domain::PlayerRecord>&)> handle_finished_players_sig;
    
    void GenerateLoot(const TimeInterval& delta_time);
    void CreateLostObject();
    void SetRandomLootType(std::shared_ptr<model::LostObject> loot);
    void LocateLootInRandomPositionOnMap(std::shared_ptr<model::LostObject> loot);
    void LocateDogInRandomPositionOnMap(std::shared_ptr<model::Dog> dog);
    void LocateDogInStartPointOnMap(std::shared_ptr<model::Dog> dog);
    void HandleLoot();
    void CollectLoot(const model::ItemDogProvider& provider, size_t item_id, size_t gatherer_id);
    void DropLoot(const model::ItemDogProvider& provider, size_t item_id, size_t gatherer_id);
    void RemoveInactiveDogs();
};

}