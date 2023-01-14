#include "model.h"
#include "json_keys.h"

#include <sstream>

namespace model {

namespace json = boost::json;

void tag_invoke(json::value_from_tag, json::value& jv, const Book& book) {
    json::object res = {{jkeys::BOOK_ID, json::value_from(book.id)}
        , {jkeys::BOOK_TITLE, json::value_from(book.title)}
        , {jkeys::BOOK_AUTHOR, json::value_from(book.author)}
        , {jkeys::BOOK_YEAR, json::value_from(book.year)}
    };
    if(book.isbn) {
        res[jkeys::BOOK_ISBN] = json::value_from(book.isbn.value());
    } else {
        res[jkeys::BOOK_ISBN] = json::value_from(NULL);
    }
    jv.emplace_object() = res;
};

Book tag_invoke(json::value_to_tag<Book>, const json::value& jv) {
    Book book;
    book.title = json::value_to<std::string>(jv.as_object().at(jkeys::BOOK_TITLE));
    book.author = json::value_to<std::string>(jv.as_object().at(jkeys::BOOK_AUTHOR));
    book.year = json::value_to<int>(jv.as_object().at(jkeys::BOOK_YEAR));
    try {
        if(!jv.as_object().at(jkeys::BOOK_ISBN).is_null()) {
            book.isbn = json::value_to<std::string>(jv.as_object().at(jkeys::BOOK_ISBN));
        }
    } catch (boost::wrapexcept<std::out_of_range>& e) {};
    return book;
};

Command tag_invoke(json::value_to_tag<Command>, const json::value& jv) {
    Command command;
    command.action = STRING_TO_ACTION.at(json::value_to<std::string>(jv.as_object().at(jkeys::COMMAND_ACTION)));
    try {
        command.payload = json::value_to<Book>(jv.as_object().at(jkeys::COMMAND_PAYLOAD));
    } catch (boost::wrapexcept<std::out_of_range>& e) {};
    return command;
};

}