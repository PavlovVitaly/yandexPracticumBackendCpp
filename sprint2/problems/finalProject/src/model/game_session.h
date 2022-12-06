#pragma once
#include "map.h"
#include "dog.h"
#include "tagged.h"

#include <chrono>
#include <vector>
#include <memory>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>

namespace model {

namespace net = boost::asio;

class GameSession {
public:
    using SessionStrand = net::strand<net::io_context::executor_type>;
    using Id = util::Tagged<std::string, GameSession>;

    GameSession(std::shared_ptr<Map> map, net::io_context& ioc) :
        map_(map),
        strand_(std::make_shared<SessionStrand>(net::make_strand(ioc))),
        id_(*(map->GetId())) {};
    
    const Id& GetId() const noexcept;
    const std::shared_ptr<Map> GetMap();
    std::shared_ptr<SessionStrand> GetStrand();
    std::shared_ptr<Dog> CreateDog(const std::string& dog_name, bool randomize_spawn_points);
private:
    std::shared_ptr<Map> map_;
    std::shared_ptr<SessionStrand> strand_;
    Id id_;
    std::vector< std::shared_ptr<Dog> > dogs_;

    void LocateDogInRandomPositionOnMap(std::shared_ptr<Dog> dog);
    void LocateDogInStartPointOnMap(std::shared_ptr<Dog> dog);
};

}