#include "middleware.h"
#include "db_functions.h"

#include <iostream>

namespace misc {

namespace json = boost::json;

std::string ReadFromConsole(){
    std::string str;
    std::getline(std::cin, str);
    return str;
};

model::Command ConvertToCommand(const std::string& str_cmd) {
    json::value root = boost::json::parse(str_cmd);
    return json::value_to<model::Command>(root);
};

std::optional<std::string> HandleCommand(const model::Command& cmd
                                        , pqxx::connection& connection
                                        , const pqxx::zview& add_book_tag) {
    switch(cmd.action){
        case model::Action::ADD_BOOK: {
            return db_fun::AddBook(cmd.payload.value()
                                , connection
                                , add_book_tag);
        }
        case model::Action::ALL_BOOKS: {
            return db_fun::AllBook(connection);
        }
        case model::Action::EXIT: {
            break;
        }
    }
    return std::nullopt;
};

}