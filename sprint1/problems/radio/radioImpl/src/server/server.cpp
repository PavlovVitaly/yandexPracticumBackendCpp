#include "server.hpp"
#include "parsingFunc.hpp"
#include <iostream>
#include <string>

namespace audioServer{

namespace net = boost::asio;
using net::ip::udp;
using namespace std::literals;

void RunServer(boost::asio::ip::udp::endpoint endpoint) {
    Player player(ma_format_u8, 1);

    try {
        boost::asio::io_context io_context;
        udp::socket socket(io_context, endpoint);

        while (true) {
            std::array<char, parseF::MAX_BUFFER_SIZE> recvBuf;
            udp::endpoint remote_endpoint;

            std::cout << "Waiting message..." << std::endl;
            auto size = socket.receive_from(boost::asio::buffer(recvBuf), remote_endpoint);
            std::cout << "Message received" << std::endl;
            Recorder::RecordingResult recResult;
            parseF::ParseMessage(recvBuf, recResult);
            player.PlayBuffer(recResult.data.data(), recResult.frames, 1.5s);
            std::cout << "Playing done" << std::endl;
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
};

}