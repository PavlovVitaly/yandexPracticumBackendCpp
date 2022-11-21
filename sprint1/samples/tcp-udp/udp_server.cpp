#include <boost/asio.hpp>
#include <array>
#include <iostream>
#include <string>
#include <string_view>

namespace net = boost::asio;
// TCP больше не нужен, импортируем имя UDP
using net::ip::udp;

using namespace std::literals;

int main() {
    static const int port = 3333;
    static const size_t max_buffer_size = 1024;

    try {
        boost::asio::io_context io_context;

        udp::socket socket(io_context, udp::endpoint(udp::v4(), port));

        // Запускаем сервер в цикле, чтобы можно было работать со многими клиентами
        for (;;) {
            // Создаём буфер достаточного размера, чтобы вместить датаграмму.
            std::array<char, max_buffer_size> recv_buf;
            udp::endpoint remote_endpoint;

            // Получаем не только данные, но и endpoint клиента
            auto size = socket.receive_from(boost::asio::buffer(recv_buf), remote_endpoint);

            std::cout << "Client said "sv << std::string_view(recv_buf.data(), size) << std::endl;

            // Отправляем ответ на полученный endpoint, игнорируя ошибку.
            // На этот раз не отправляем перевод строки: размер датаграммы будет получен автоматически.
            boost::system::error_code ignored_error;
            socket.send_to(boost::asio::buffer("Hello from UDP-server"sv), remote_endpoint, 0, ignored_error);
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}