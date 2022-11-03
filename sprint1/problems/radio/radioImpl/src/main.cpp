#include "audio.h"
#include <iostream>
#include <boost/asio.hpp>
#include <sstream>
#include <regex>

namespace net = boost::asio;
using net::ip::udp;
using namespace std::literals;

const size_t MAX_BUFFER_SIZE = 65100;

/*
    Functions for packing and unpacking Recorder::RecordingResult data to buffer. 
    Structure of packed Recorder::RecordingResult:
        [0]: 0xAA header byte #1.
        [1]: 0x55 header byte #2.
        [2]: size of size_t type in bytes.
        [3] - [ (3 + [2] - 1) ]: full size of packed data (size of transmitting buffer) type in bytes.
        [ (3 + [2] - 1) ] - [ (3 + 2*[2] - 1) ]: frames field of Recorder::RecordingResult type in bytes.
        [ (3 + 2*[2] - 1) ] - [n]: data field of Recorder::RecordingResult.

    * Header 0xAA 0x55 for filtering trash data. I think no really need use checksum in this case.
*/

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

bool ParseMessage(const std::array<char, MAX_BUFFER_SIZE>& msg, Recorder::RecordingResult& record){
    /*Filtering trash data and clients that use size_t which is not the same size as size_t on the server side.*/
    if(msg[0] != static_cast<char>(0xAA) or
        msg[1] != static_cast<char>(0x55) or
        msg[2] != static_cast<char>(sizeof(size_t))) return false; 
    /*Write frames data*/
    size_t* frames = (size_t*)(&msg[3 + msg[2]]);
    record.frames = *frames;
    /*Calculating of data size*/
    size_t* msgSize = (size_t*)(&msg[3]);
    size_t headSize = 3 + 2*msg[2]; // Size of header bytes + byte with size of size_t type + message size value + frames value.
    size_t dataSize = *msgSize - headSize;
    /*Write data*/
    for (size_t i = 0; i < dataSize; ++i){
        record.data.push_back(msg[headSize + i]);
    }
    return true;
}

bool ValidatePort(size_t port){
    return port < 65535;
}

void RunServer(boost::asio::ip::udp::endpoint endpoint) {
    Player player(ma_format_u8, 1);

    try {
        boost::asio::io_context io_context;
        udp::socket socket(io_context, endpoint);

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
};

void RunClient(size_t port) {
    Recorder recorder(ma_format_u8, 1);
    net::io_context io_context;
    udp::socket socket(io_context, udp::v4());
    boost::system::error_code ec;
    std::vector<char> data;
    std::regex ipRegex(R"((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?))");
    std::smatch regexRes{};


    std::string previousIp;
    while (true) {
        std::string str;
        std::string ip;
        
        std::cout << "Enter server ip or press enter for using previous server ip." << std::endl;
        std::getline(std::cin, ip);
        if(!ip.empty() and !std::regex_match(ip, regexRes, ipRegex)){
            std::cout << "Invalid ip." << std::endl;
            continue;
        }
        else if(ip.empty() and !previousIp.empty()) ip =  previousIp;
        else if(ip.empty() and previousIp.empty()){
            std::cout << "No server previous ip. Enter server ip." << std::endl;
            continue;
        }
        auto endpoint = udp::endpoint(net::ip::address_v4::from_string(ip, ec), port);
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

        if(!ip.empty() and ip != previousIp){
            previousIp = ip;
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "Usage: "sv << argv[0] << " <server IP>"sv << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    std::transform(mode.begin(), mode.end(), mode.begin(),
        [](unsigned char c){ return std::toupper(c); });
    
    std::stringstream sstream(argv[2]);
    size_t port;
    sstream >> port;

    if(!ValidatePort(port)){
        std::cout << "Wrong port" << std::endl;
        std::exit(1);
    }
    
    std::cout << "Set mode: " << mode << " . Set port: " << port << std::endl;

    if(mode == "CLIENT"){
        RunClient(port);
    }else if(mode == "SERVER"){
        RunServer(udp::endpoint(udp::v4(), port));
    }

    std::cout << "Mode: " << mode << " doesn't existing. Availible modes: client and server." << std::endl;

    return 0;
}
