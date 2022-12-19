#pragma once
#include "map.h"
//#include "player.h"
#include "tagged.h"
#include "loot_generator_config.h"
#include "loot_generator.h"
#include "lost_object.h"

#include <chrono>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_set>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>

namespace app {

namespace net = boost::asio;

class GameSession {
public:
    using SessionStrand = net::strand<net::io_context::executor_type>;
    using Id = util::Tagged<std::string, GameSession>;
    using TimeInterval = std::chrono::milliseconds;

    GameSession(std::shared_ptr<model::Map> map,
                const model::LootGeneratorConfig& loot_gen_cfg,
                net::io_context& ioc) :
        map_(map),
        strand_(std::make_shared<SessionStrand>(net::make_strand(ioc))),
        id_(*(map->GetId())),
        loot_generator_(
            TimeInterval(static_cast<uint64_t>(
                loot_gen_cfg.period * model::MILLISECONDS_IN_SECOND)
            ),
            loot_gen_cfg.probability) {};
    
    const Id& GetId() const noexcept;
    const std::shared_ptr<model::Map> GetMap();
    std::shared_ptr<SessionStrand> GetStrand();
    
private:
    std::shared_ptr<model::Map> map_;
    std::shared_ptr<SessionStrand> strand_;
    Id id_;
    loot_gen::LootGenerator loot_generator_;
    //std::unordered_set< std::shared_ptr<Player> > players_;
    //std::unordered_set<model::LostObject> lost_objects_;
};

}