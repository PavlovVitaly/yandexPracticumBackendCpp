#include "sdk.h"
//
#include "json_loader.h"
#include "request_handler.h"
#include "logger.h"
#include "application.h"
#include "program_options.h"
#include "saving_settings.h"
#include "database_invariants.h"
#include "database_exceptions.h"
#include "database_connection_settings.h"

#include <boost/asio/io_context.hpp>
#include <thread>
#include <boost/asio/signal_set.hpp>
#include <filesystem>
#include <chrono>
#include <memory>

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
    // 0. Инициализация логгера.
    logware::InitLogger();
    prog_opt::Args args = prog_opt::ParseCommandLine(argc, argv);
    const unsigned num_threads = std::thread::hardware_concurrency();
    try {
        // 1. Прочитать из переменной среды url базы данных
        const char* db_url = std::getenv(db_invariants::DB_URL.c_str());
        if (!db_url) {
            throw db_ex::EmptyDatabaseUrl();
        }
        db_conn_settings::DbConnectrioSettings db_settings{num_threads, std::move(db_url)};
        
        // 2. Загружаем карту из файла и построить модель игры
        model::Game game = json_loader::LoadGame(args.config_file);

        // 3. Устанавливаем путь до статического контента.
        fs::path sc_root_path{args.www_root};
        //fs::path sc_root_path{"../../static"};

        // 4. Инициализируем io_context
        net::io_context ioc(num_threads);

        // 5. Создание application
        auto application = std::make_shared<app::Application>(std::move(game)
                                                            , args.tick_period
                                                            , args.randomize_spawn_points
                                                            , ioc
                                                            , std::move(db_settings));

        // 6. Инициализация настроек сохранения игрового состояния.
        saving::SavingSettings saving_settings;
        if(!args.state_file.empty()) {
            saving_settings.state_file_path = args.state_file;
            if(args.save_state_period != 0) {
                saving_settings.period = std::chrono::milliseconds(args.save_state_period);
            }
            application->RestoreGameState(std::move(saving_settings));
        }        

        // 7. Добавляем асинхронный обработчик сигналов SIGINT и SIGTERM
        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc, application](const sys::error_code& ec, [[maybe_unused]] int signal_number) {
            if (!ec) {
                BOOST_LOG_TRIVIAL(info) << logware::CreateLogMessage("server exited"sv,
                                                                        logware::ExitCodeLogData(0));
                application->SaveGame();
                ioc.stop();
            }
        });

        // 8. Создаём обработчик HTTP-запросов и связываем его с моделью игры, задаем путь до статического контента.
        http_handler::RequestHandler handler{application, sc_root_path};

        // 9. Запустить обработчик HTTP-запросов, делегируя их обработчику запросов
        const auto address = net::ip::make_address("0.0.0.0");
        constexpr net::ip::port_type port = 8080;
        http_server::ServeHttp(ioc, {address, port}, [&handler](auto&& req, auto&& send) {
            handler(std::forward<decltype(req)>(req), std::forward<decltype(send)>(send));
        });
        
        // Эта надпись сообщает тестам о том, что сервер запущен и готов обрабатывать запросы
        BOOST_LOG_TRIVIAL(info) << logware::CreateLogMessage("Server has started..."sv,
                                                                logware::ServerAddressLogData(address.to_string(), port));

        // 10. Запускаем обработку асинхронных операций
        RunWorkers(std::max(1u, num_threads), [&ioc] {
            ioc.run();
        });
    } catch (const std::exception& ex) {
        BOOST_LOG_TRIVIAL(error) << logware::CreateLogMessage("error"sv,
                                        logware::ExceptionLogData(EXIT_FAILURE, "Server down"sv, ex.what()));
        return EXIT_FAILURE;
    }
}
