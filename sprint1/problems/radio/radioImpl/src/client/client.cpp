#include "client.hpp"
#include "parsingFunc.hpp"
#include <iostream>
#include <string>

namespace audioServer{

namespace net = boost::asio;
using net::ip::udp;
using namespace std::literals;

void RunClient(size_t port) {
    Recorder recorder(ma_format_u8, 1);
    net::io_context io_context;
    udp::socket socket(io_context, udp::v4());
    boost::system::error_code ec;
    std::vector<char> data;

    std::string previousIp;
    while (true) {
        std::string str;
        std::string ip;
        
        std::cout << "Enter server ip or press enter for using previous server ip." << std::endl;
        std::getline(std::cin, ip);
        if(ip.empty() and !previousIp.empty()) ip =  previousIp;
        else if(ip.empty() and previousIp.empty()){
            std::cout << "No server previous ip. Enter server ip." << std::endl;
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

}