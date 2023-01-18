#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "bookypedia.h"

using namespace std::literals;

namespace {

constexpr const char DB_URL_ENV_NAME[]{"BOOKYPEDIA_DB_URL"};

bookypedia::AppConfig GetConfigFromEnv() {
    bookypedia::AppConfig config;
    if (const auto* url = std::getenv(DB_URL_ENV_NAME)) {
        config.db_url = url;
    } else {
        throw std::runtime_error(DB_URL_ENV_NAME + " environment variable not found"s);
    }
    return config;
}

}  // namespace

int main([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[]) {
    try {
        bookypedia::Application app{GetConfigFromEnv()};

        //for testing
        //bookypedia::AppConfig config;
        //config.db_url = "postgres://postgres:Mys3Cr3t@localhost:30432/yandex_test";
        //bookypedia::Application app{config};
        
        app.Run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
