#pragma once
#include "map.h"
#include "dog.h"
#include "tagged.h"
#include "loot_generator_config.h"
#include "loot_generator.h"
#include "lost_object.h"
#include "ticker.h"

#include <chrono>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>

namespace app {

namespace net = boost::asio;

class GameSession {
public:
    using SessionStrand = net::strand<net::io_context::executor_type>;
    using Id = util::Tagged<std::string, GameSession>;
    using TimeInterval = std::chrono::milliseconds;
    using LostObjectIdHasher = util::TaggedHasher<model::LostObject::Id>;
    using LostObjects = std::unordered_map<model::LostObject::Id,
                        std::shared_ptr<model::LostObject>,
                        LostObjectIdHasher>;

    GameSession(std::shared_ptr<model::Map> map,
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
        if(period_of_update_game_state_.count() != 0){
            update_game_state_ticker_ = std::make_shared<time_m::Ticker>(
                strand_,
                period_of_update_game_state_,
                std::bind(&GameSession::UpdateGameState, this, std::placeholders::_1)
            );
            update_game_state_ticker_->Start();
        }

        generate_loot_ticker_ = std::make_shared<time_m::Ticker>(
            strand_,
            loot_generator_.GetPeriod(),
            std::bind(&GameSession::GenerateLoot, this, std::placeholders::_1)
        );
        generate_loot_ticker_->Start();
    };
    
    const Id& GetId() const noexcept;
    const std::shared_ptr<model::Map> GetMap();
    std::shared_ptr<SessionStrand> GetStrand();
    std::weak_ptr<model::Dog> CreateDog(const std::string& dog_name,
                                        const model::Map& map,
                                        bool randomize_spawn_points);
    void UpdateGameState(const TimeInterval& delta_time);
    const LostObjects& GetLostObjects();
    
private:
    using DogIdHasher = util::TaggedHasher<model::Dog::Id>;

    std::shared_ptr<model::Map> map_;
    net::io_context& ioc_;
    std::shared_ptr<SessionStrand> strand_;
    Id id_;
    loot_gen::LootGenerator loot_generator_;
    std::unordered_map<model::Dog::Id,
                        std::shared_ptr<model::Dog>,
                        DogIdHasher> dogs_;
    LostObjects lost_objects_;
    TimeInterval period_of_update_game_state_;
    std::shared_ptr<time_m::Ticker> update_game_state_ticker_;
    std::shared_ptr<time_m::Ticker> generate_loot_ticker_;
    
    void GenerateLoot(const TimeInterval& delta_time);
    void CreateLostObject();
    void SetRandomLootType(std::shared_ptr<model::LostObject> loot);
    void LocateLootInRandomPositionOnMap(std::shared_ptr<model::LostObject> loot);
    void LocateDogInRandomPositionOnMap(std::shared_ptr<model::Dog> dog);
    void LocateDogInStartPointOnMap(std::shared_ptr<model::Dog> dog);
};

}