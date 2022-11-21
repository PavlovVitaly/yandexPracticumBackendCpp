#include "sdk.h"
//
#include <boost/asio/io_context.hpp>
#include <iostream>
#include <thread>
#include <boost/asio/signal_set.hpp>
#include <filesystem>

#include "json_loader.h"
#include "request_handler.h"

using namespace std::literals;
namespace net = boost::asio;
namespace sys = boost::system;
namespace fs = std::filesystem;

namespace {

// Запускает функцию fn на n потоках, включая текущий
template <typename Fn>
void RunWorkers(size_t n, const Fn& fn) {
    n = std::max(static_cast<size_t>(1), n);
    std::vector<std::jthread> workers;
    workers.reserve(n - 1);
    // Запускаем n-1 рабочих потоков, выполняющих функцию fn
    while (--n) {
        workers.emplace_back(fn);
    }
    fn();
}

}  // namespace

int main(int argc, const char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: game_server <game-config-json>"sv << std::endl;
        return EXIT_FAILURE;
    }
    try {
        // 1. Загружаем карту из файла и построить модель игры
        model::Game game = json_loader::LoadGame(argv[1]);
        //model::Game game = json_loader::LoadGame("../../data/config.json"); // for debug

        // 2. Устанавливаем путь до статического контента.
        fs::path sc_root_path{argv[2]};
        //fs::path sc_root_path{"../../static"};

        // 3. Инициализируем io_context
        const unsigned num_threads = std::thread::hardware_concurrency();
        net::io_context ioc(num_threads);

        // 4. Добавляем асинхронный обработчик сигналов SIGINT и SIGTERM
        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const sys::error_code& ec, [[maybe_unused]] int signal_number) {
            if (!ec) {
                std::cout << "Signal "sv << signal_number << " received"sv << std::endl;
                ioc.stop();
            }
        });

        // 5. Создаём обработчик HTTP-запросов и связываем его с моделью игры, задаем путь до статического контента.
        http_handler::RequestHandler handler{game, sc_root_path};

        // 6. Запустить обработчик HTTP-запросов, делегируя их обработчику запросов
        const auto address = net::ip::make_address("0.0.0.0");
        constexpr net::ip::port_type port = 8080;
        http_server::ServeHttp(ioc, {address, port}, [&handler](auto&& req, auto&& send) {
            handler(std::forward<decltype(req)>(req), std::forward<decltype(send)>(send));
        });
        
        // Эта надпись сообщает тестам о том, что сервер запущен и готов обрабатывать запросы
        std::cout << "Server has started..."sv << std::endl;

        // 7. Запускаем обработку асинхронных операций
        RunWorkers(std::max(1u, num_threads), [&ioc] {
            ioc.run();
        });
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
}
