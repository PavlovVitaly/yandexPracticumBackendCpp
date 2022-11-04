#include "audio.h"
#include <iostream>
#include <boost/asio.hpp>
#include <sstream>
#include <regex>

namespace net = boost::asio;
using net::ip::udp;
using namespace std::literals;

bool ValidatePort(size_t port){
    const size_t MAX_PORTS_NUMBER = 65535;
    return port < MAX_PORTS_NUMBER;
}

void RunServer(boost::asio::ip::udp::endpoint endpoint, const size_t framesNumber) {
    Player player(ma_format_u8, 1);
    const size_t dataSizeInBytes = framesNumber * player.GetFrameSize();
    try {
        boost::asio::io_context io_context;
        udp::socket socket(io_context, endpoint);
        while (true) {
            udp::endpoint remote_endpoint;
            Recorder::RecordingResult recResult;
            recResult.data.resize(dataSizeInBytes);
            recResult.frames = framesNumber;
            std::cout << "Waiting message..." << std::endl;
            auto size = socket.receive_from(boost::asio::buffer(recResult.data), remote_endpoint);
            std::cout << "Message received" << std::endl;
            player.PlayBuffer(recResult.data.data(), recResult.frames, 1.5s);
            std::cout << "Playing done" << std::endl;
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
};

void RunClient(size_t port, const size_t framesNumber) {
    Recorder recorder(ma_format_u8, 1);
    const size_t dataSizeInBytes = framesNumber * recorder.GetFrameSize();
    net::io_context io_context;
    udp::socket socket(io_context, udp::v4());
    boost::system::error_code ec;
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

        auto recResult = recorder.Record(framesNumber, 1.5s);
        std::cout << "Recording done" << std::endl;
       
        try{
            socket.send_to(boost::asio::buffer(recResult.data), endpoint);
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
    const size_t framesNumber = 65000;
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
        RunClient(port, framesNumber);
    }else if(mode == "SERVER"){
        RunServer(udp::endpoint(udp::v4(), port), framesNumber);
    }

    std::cout << "Mode: " << mode << " doesn't existing. Availible modes: client and server." << std::endl;
    return 0;
}