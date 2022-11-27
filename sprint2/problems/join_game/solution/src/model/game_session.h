#pragma once
#include "map.h"
#include "dog.h"

#include <vector>
#include <memory>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>

namespace model {

namespace net = boost::asio;

class GameSession {
public:
    using SessionStrand = net::strand<net::io_context::executor_type>;

    GameSession(std::shared_ptr<Map> map, net::io_context& ioc) :
        map_(map),
        strand_(std::make_shared<SessionStrand>(net::make_strand(ioc))) {};
    
    const std::shared_ptr<Map> GetMap();
    std::shared_ptr<SessionStrand> GetStrand();
    std::shared_ptr<Dog> CreateDog(const std::string& dog_name);

private:
    std::shared_ptr<Map> map_;
    std::shared_ptr<SessionStrand> strand_;
    std::vector< std::shared_ptr<Dog> > dogs_;
};

}