#include "audio.h"
#include <boost/asio.hpp>
#include <array>
#include <iostream>
#include <string>
#include <string_view>

namespace net = boost::asio;
using net::ip::udp;
using namespace std::literals;

static const size_t MAX_BUFFER_SIZE = 65100;

bool ParseMessage(const std::array<char, MAX_BUFFER_SIZE>& msg, Recorder::RecordingResult& record){
    if(msg[0] != static_cast<char>(0xAA) or
        msg[1] != static_cast<char>(0x55) or
        msg[2] != static_cast<char>(sizeof(size_t))) return false; 
    size_t* frames = (size_t*)(&msg[3 + msg[2]]);
    record.frames = *frames;
    size_t* msgSize = (size_t*)(&msg[3]);
    size_t headSize = 3 + 2*msg[2];
    size_t dataSize = *msgSize - headSize;
    for (size_t i = 0; i < dataSize; ++i){
        record.data.push_back(msg[headSize + i]);
    }
    return true;
};

int main(int argc, char** argv) {
    Player player(ma_format_u8, 1);
    static const int PORT = 3333;

    try {
        boost::asio::io_context io_context;
        udp::socket socket(io_context, udp::endpoint(udp::v4(), PORT));

        while (true) {
            std::array<char, MAX_BUFFER_SIZE> recvBuf;
            udp::endpoint remote_endpoint;

            std::cout << "Waiting message..." << std::endl;
            auto size = socket.receive_from(boost::asio::buffer(recvBuf), remote_endpoint);
            std::cout << "Message received" << std::endl;
            Recorder::RecordingResult recResult;
            ParseMessage(recvBuf, recResult);
            player.PlayBuffer(recResult.data.data(), recResult.frames, 1.5s);
            std::cout << "Playing done" << std::endl;
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
