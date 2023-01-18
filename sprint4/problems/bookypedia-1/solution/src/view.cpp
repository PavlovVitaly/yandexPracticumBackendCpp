#include "view.h"

#include <boost/algorithm/string/trim.hpp>
#include <iostream>
#include <string>

#include "menu.h"
#include "use_cases.h"

using namespace std::literals;
namespace ph = std::placeholders;

namespace ui {

View::View(menu::Menu& menu, app::UseCases& use_cases, std::istream& input, std::ostream& output)
    : menu_{menu}
    , use_cases_{use_cases}
    , input_{input}
    , output_{output} {
    menu_.AddAction(  //
        "AddAuthor"s, "name"s, "Adds author"s, //std::bind(&View::AddAuthor, this, ph::_1)
        // ����
         [this](auto& cmd_input) { return AddAuthor(cmd_input); }
    );
    menu_.AddAction(  //
        "ShowAuthors"s, ""s, "Show authors"s, [this](auto& cmd_input) { return ShowAuthors(); }
    );
    menu_.AddAction(  //
        "AddBook"s, "title, year"s, "Adds book"s, [this](auto& cmd_input) { return AddBook(cmd_input); }
    );
    menu_.AddAction(  //
        "ShowBooks"s, ""s, "Show books"s, [this](auto& cmd_input) { return ShowBooks(); }
    );
}

bool View::AddAuthor(std::istream& cmd_input) {
    try {
        std::string name;
        std::getline(cmd_input, name);
        boost::algorithm::trim(name);
        use_cases_.AddAuthor(std::move(name));
    } catch (const std::exception&) {
        output_ << "Failed to add author"sv << std::endl;
    }
    return true;
}

bool View::ShowAuthors() {
    try {
        size_t count = 1;
        auto list_of_authors = use_cases_.GetAllAuthors();
        for(auto& item : list_of_authors) {
            output_ << count++ << ". " << item << std::endl; 
        }
    } catch (const std::exception&) {
        output_ << "Failed to show authors"sv << std::endl;
    }
    return true;
}

bool View::AddBook(std::istream& cmd_input) {
    try {
        std::string title;
        int year{0};
        cmd_input >> year;
        std::getline(cmd_input, title);
        boost::algorithm::trim(title);
        output_ << std::endl;
        
        size_t count = 1;
        auto list_of_authors = use_cases_.GetAllAuthors();
        output_ << "Select author:" << std::endl;
        for(auto& item : list_of_authors) {
            output_ << count++ << ". " << item << std::endl; 
        }

        int index_of_choosed_author{0};
        do {
            std::cin >> index_of_choosed_author;
            if((index_of_choosed_author <= 0)
                or (index_of_choosed_author > list_of_authors.size())) {
                output_ << "Invalid author. Retry attempt."sv << std::endl;
            }
        } while((index_of_choosed_author <= 0)
                or (index_of_choosed_author > list_of_authors.size()));
        
        auto id_of_choosed_author = use_cases_.GetAuthorIdBy(list_of_authors[index_of_choosed_author - 1]);
        if(!id_of_choosed_author){
            output_ << "Author doesn't exist. Failed to add book"sv << std::endl;
            return false;
        }
        
        use_cases_.AddBook(*id_of_choosed_author, std::move(title), year);
    } catch (const std::exception&) {
        output_ << "Failed to add book"sv << std::endl;
        return false;
    }
    return true;
}

bool View::ShowBooks() {
    try {
        size_t count = 1;
        auto list_of_books = use_cases_.GetAllBooks();
        for(auto& item : list_of_books) {
            output_ << count++ << ". " << item << std::endl; 
        }
    } catch (const std::exception&) {
        output_ << "Failed to show books"sv << std::endl;
    }
    return true;
}

}  // namespace ui
