#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <string_view>

namespace net = boost::asio;
using net::ip::udp;

using namespace std::literals;

int main(int argc, const char** argv) {
    static const int port = 3333;
    static const size_t max_buffer_size = 1024;

    if (argc != 2) {
        std::cout << "Usage: "sv << argv[0] << " <server IP>"sv << std::endl;
        return 1;
    }

    try {
        boost::asio::io_context io_context;

        // Перед отправкой данных нужно открыть сокет. 
        // При открытии указываем протокол (IPv4 или IPv6) вместо endpoint.
        udp::socket socket(io_context, udp::v4());

        boost::system::error_code ec;
        auto endpoint = udp::endpoint(net::ip::address_v4::from_string(argv[1], ec), port);
        socket.send_to(boost::asio::buffer("Hello from UDP-client"sv), endpoint);

        // Получаем данные и endpoint.
        std::array<char, max_buffer_size> recv_buf;
        udp::endpoint sender_endpoint;
        size_t size = socket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);

        std::cout << "Server responded "sv << std::string_view(recv_buf.data(), size) << std::endl;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}