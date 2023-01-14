#pragma once

#include <string>
#include <optional>
#include <unordered_map>
#include <boost/json.hpp>

namespace model {

namespace json = boost::json;

struct Book {
    int id{0};
    std::string title{};
    std::string author{};
    int year{0};
    std::optional<std::string> isbn;
};

void tag_invoke(json::value_from_tag, json::value& jv, const Book& book);
Book tag_invoke(json::value_to_tag<Book>, const json::value& jv);

enum class Action {ADD_BOOK, ALL_BOOKS, EXIT};

const std::unordered_map<std::string, Action> STRING_TO_ACTION = {
    {"add_book", Action::ADD_BOOK},
    {"all_books", Action::ALL_BOOKS},
    {"exit", Action::EXIT},
};

struct Command {
    Action action{};
    std::optional<Book> payload;
};

Command tag_invoke(json::value_to_tag<Command>, const json::value& jv);

}