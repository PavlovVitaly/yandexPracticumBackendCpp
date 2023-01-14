// main.cpp
#include "db_functions.h"
#include "middleware.h"

#include <iostream>
#include <pqxx/pqxx>

using namespace std::literals;
// libpqxx использует zero-terminated символьные литералы вроде "abc"_zv;
using pqxx::operator"" _zv;

int main(int argc, const char* argv[]) {
    if (argc == 1) {
        std::cout << "Usage: connect_db <conn-string>\n"sv;
        return EXIT_SUCCESS;
    } else if (argc != 2) {
        std::cerr << "Invalid command line\n"sv;
        return EXIT_FAILURE;
    }
    // Подключаемся к БД, указывая её параметры в качестве аргумента
    pqxx::connection conn{argv[1]};
    db_fun::CreateBooksTable(conn);
    constexpr auto add_book_tag = "add_book"_zv;
    db_fun::PreapareAddBookCommand(conn, add_book_tag);

    while(true) {
        auto result = misc::HandleCommand(
            misc::ConvertToCommand(
                misc::ReadFromConsole()
            ), 
            conn,
            add_book_tag
        );
        if(!result) {
            return 0;
        }
        std::cout << result.value() << std::endl;
    }
}
