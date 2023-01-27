#pragma once

#include <string>

namespace db_conn_settings {

struct DbConnectrioSettings {
    size_t number_of_connection{1};
    std::string db_url{};
};

}