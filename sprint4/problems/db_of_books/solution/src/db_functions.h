#pragma once

#include "model.h"

#include <vector>
#include <pqxx/pqxx>

namespace db_fun {

void CreateBooksTable(pqxx::connection& connection);
void PreapareAddBookCommand(pqxx::connection& connection, const pqxx::zview& tag);
std::string AddBook(const model::Book& book, pqxx::connection& connection, const pqxx::zview& tag);
std::string AllBook(pqxx::connection& connection);
std::string ConvertToJsonString(const std::vector<model::Book>& books);
std::string GetResultMsg(bool is_success = true);

}