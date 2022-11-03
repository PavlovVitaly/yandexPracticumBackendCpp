#pragma once

#include <boost/asio.hpp>

namespace audioServer{

void RunServer(boost::asio::ip::udp::endpoint endpoint);

}