#include "db_functions.h"
#include "json_keys.h"

#include <iostream>
#include <boost/json.hpp>

namespace db_fun {

namespace json = boost::json;

using namespace std::literals;
// libpqxx использует zero-terminated символьные литералы вроде "abc"_zv;
using pqxx::operator"" _zv;

void CreateBooksTable(pqxx::connection& connection) {
    try {
        // Создаём транзакцию. Это понятие будет разобрано в следующих уроках.
        // Транзакция нужна, чтобы выполнять запросы.
        pqxx::work transaction(connection);
        // Используя транзакцию создадим таблицу в выбранной базе данных:
        transaction.exec(
            "CREATE TABLE IF NOT EXISTS books (id SERIAL PRIMARY KEY, title varchar(100) NOT NULL, author varchar(100) NOT NULL, year integer NOT NULL, ISBN char(13) UNIQUE);"_zv);
        transaction.exec("DELETE FROM books;"_zv);
        // Применяем все изменения
        transaction.commit();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return;
    }
};

void PreapareAddBookCommand(pqxx::connection& connection, const pqxx::zview& tag) {
    connection.prepare(tag, "INSERT INTO books (title, author, year, ISBN) VALUES ($1, $2, $3, $4)"_zv);
};

std::string AddBook(const model::Book& book, pqxx::connection& connection, const pqxx::zview& tag) {
    std::string result;
    try {
        pqxx::work transaction(connection);
        transaction.exec_prepared(tag
            , book.title
            , book.author
            , book.year
            , book.isbn);
        transaction.commit();
        result = GetResultMsg();
    } catch (const std::exception& e) {
        result = GetResultMsg(false);
    }
    return result;
};

std::string AllBook(pqxx::connection& connection) {
    std::string result;
    try {
        auto query_text = "SELECT * FROM books ORDER BY year DESC, title ASC, author ASC, ISBN ASC"_zv;
        pqxx::read_transaction r(connection);
        std::vector<model::Book> books;
        // Выполняем запрос и итерируемся по строкам ответа
        for (auto [id, title, author, year, isbn] : r.query<int, std::string, std::string, int, std::optional<std::string>>(query_text)) {
            books.emplace_back(id, title, author, year, isbn);
        }
        result = ConvertToJsonString(books);
    } catch (const std::exception& e) {
        result = GetResultMsg(false);
    }
    return result;
};

std::string ConvertToJsonString(const std::vector<model::Book>& books) {
    return json::serialize(json::value_from(books));
};

std::string GetResultMsg(bool is_success) {
    json::value msg = {{jkeys::RESULT, is_success}};
    return json::serialize(msg);
};

}