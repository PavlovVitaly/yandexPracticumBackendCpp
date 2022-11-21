#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <string_view>

namespace net = boost::asio;
using net::ip::tcp;
using namespace std::literals;

int main(int argc, char** argv) {
    static const int port = 3333;

    if (argc != 2) {
        std::cout << "Usage: "sv << argv[0] << " <server IP>"sv << std::endl;
        return 1;
    }

    // Создадим endpoint - объект с информацией об адресе и порте.
    // Для разбора IP-адреса пользуемся функцией net::ip::address_v4::from_string.
    boost::system::error_code ec;
    auto endpoint = tcp::endpoint(net::ip::make_address(argv[1], ec), port);

    if (ec) {
        std::cout << "Wrong IP format"sv << std::endl;
        return 1;
    }

    net::io_context io_context;
    tcp::socket socket{io_context};
    socket.connect(endpoint, ec);

    if (ec) {
        std::cout << "Can't connect to server"sv << std::endl;
        return 1;
    }

    socket.write_some(net::buffer("Hello, I'm client!\n"sv), ec);
    if (ec) {
        std::cout << "Error sending data"sv << std::endl;
        return 1;
    }

    net::streambuf stream_buf;
    net::read_until(socket, stream_buf, '\n', ec);
    std::string server_data{std::istreambuf_iterator<char>(&stream_buf),
                            std::istreambuf_iterator<char>()};
    if (ec) {
        std::cout << "Error reading data"sv << std::endl;
        return 1;
    }

    std::cout << "Server responded: "sv << server_data << std::endl;
}