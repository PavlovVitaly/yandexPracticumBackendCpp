#include "audio.h"
#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <algorithm>

using namespace std::literals;
namespace net = boost::asio;
using net::ip::udp;

void FillMessage(std::vector<char>& msg, const Recorder::RecordingResult& record){
    msg.clear();
    msg.push_back(0xAA); // header byte #1 (byte #0).
    msg.push_back(0x55); // header byte #2 (byte #1).
    const char size_tSizeInBytes = sizeof(size_t); 
    msg.push_back(sizeof(size_t)); // size_t size in bytes (byte #2).
    /*Allocate message size in bytes to buffer*/
    for(size_t i = 0; i < size_tSizeInBytes; ++i){
        msg.push_back(0);
    }
    /*Write frames to buffer*/
    char* framesToCharArr = (char*)(&record.frames);
    for(size_t i = 0; i < size_tSizeInBytes; ++i){
        msg.push_back(framesToCharArr[i]);
    }
    /*Write data to buffer*/
    std::copy(record.data.begin(), record.data.end(), std::back_inserter(msg));
    size_t msgSize = msg.size();
    /*Write message size in bytes to buffer*/
    char* msgSizeToCharArr = (char*)(&msgSize);
    for(size_t i = 0; i < size_tSizeInBytes; ++i){
        msg[3 + i] = msgSizeToCharArr[i];
    }
}

/*bool ParseMessage(const std::vector<char>& msg, Recorder::RecordingResult& record){
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
};*/


int main(int argc, char** argv) {
    Recorder recorder(ma_format_u8, 1);
    static const int port = 3333;
    net::io_context io_context;
    udp::socket socket(io_context, udp::v4());
    boost::system::error_code ec;
    std::vector<char> data;

    Player player(ma_format_u8, 1);

    //auto endpoint = udp::endpoint(net::ip::address_v4::from_string(argv[1], ec), port);
    //if (argc != 2) {
    //    std::cout << "Usage: "sv << argv[0] << " <server IP>"sv << std::endl;
    //    return 1;
    //}
    auto endpoint = udp::endpoint(net::ip::address_v4::from_string("127.0.0.1", ec), port);

    while (true) {
        std::string str;
        
        std::cout << "Press Enter to record message..." << std::endl;
        std::getline(std::cin, str);

        auto recResult = recorder.Record(65000, 1.5s);
        std::cout << "Recording done" << std::endl;
        FillMessage(data, recResult);
       
        try{
            socket.send_to(boost::asio::buffer(data), endpoint);
            std::cout << "Message is sent" << std::endl;
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    return 0;
}
