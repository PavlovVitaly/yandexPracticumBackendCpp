#pragma once

#include "model.h"

#include <sstream>
#include <optional>
#include <pqxx/pqxx>

namespace misc {

std::string ReadFromConsole();
model::Command ConvertToCommand(const std::string& str_cmd);
std::optional<std::string> HandleCommand(const model::Command& cmd
                                        , pqxx::connection& connection
                                        , const pqxx::zview& add_book_tag);

}