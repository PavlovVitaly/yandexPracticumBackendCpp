#include <iostream>
#include <string>
#include <string_view>

#include <boost/asio.hpp>

namespace net = boost::asio;
using net::ip::tcp;

using namespace std::literals; 

int main(){
    static const int port = 3333;

    net::io_context io_context;

    // используем конструктор tcp::v4 по умолчанию для адреса 0.0.0.0
    //tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
    tcp::acceptor acceptor(io_context, tcp::endpoint(net::ip::make_address("127.0.0.1"), port));
    boost::system::error_code ec;
    tcp::socket socket{io_context};

    std::cout << "Waiting for connection..."sv << std::endl;
    acceptor.accept(socket, ec);
    if (ec) {
        std::cout << "Can't accept connection"sv << std::endl;
        return 1;
    }

    net::streambuf stream_buf;
    net::read_until(socket, stream_buf, '\n', ec);
    std::string client_data{std::istreambuf_iterator<char>(&stream_buf),
                            std::istreambuf_iterator<char>()};

    if (ec) {
        std::cout << "Error reading data"sv << std::endl;
        return 1;
    }

    std::cout << "Client said: "sv << client_data << std::endl;

    socket.write_some(net::buffer("Hello, I'm server!\n"sv), ec);

    if (ec) {
        std::cout << "Error sending data"sv << std::endl;
        return 1;
    }
}