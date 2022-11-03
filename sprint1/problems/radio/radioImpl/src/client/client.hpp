#pragma once

#include <boost/asio.hpp>

namespace audioClient{

void RunClient(boost::asio::ip::udp::endpoint endpoint);

}