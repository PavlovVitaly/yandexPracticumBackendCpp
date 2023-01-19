#include "use_cases_impl.h"
#include "author.h"
#include "book.h"

#include <ranges>
#include <algorithm>
#include <sstream>
#include <optional>

namespace app {
using namespace domain;

void UseCasesImpl::AddAuthor(const std::string& name) {
    authors_.Save({AuthorId::New(), name});
}

std::vector<std::string> UseCasesImpl::GetAllAuthors(){
    std::vector<std::string> list_of_authors;
    std::ranges::transform(
        authors_.GetAllAuthors(),
        std::back_inserter(list_of_authors),
        [](auto& author) -> std::string {
            return author.GetName();
        }
    );
    return list_of_authors;
}

std::optional<std::string> UseCasesImpl::GetAuthorIdBy(const std::string& author_name) {
    auto author = authors_.GetAuthorBy(author_name);
    if(author) {
        return author.value().GetId().ToString();
    }
    return std::nullopt;
};

void UseCasesImpl::AddBook(const std::string& author_id, const std::string& title, int year) {
    books_.Save({BookId::New(), AuthorId::FromString(author_id), title, year});
};

std::vector<std::string> UseCasesImpl::GetAllBooks() {
    std::vector<std::string> list_of_books;
    std::ranges::transform(
        books_.GetAllBooks(),
        std::back_inserter(list_of_books),
        [](auto& book) -> std::string {
            std::stringstream ss;
            ss << book.GetTitle() << ", " << book.GetPublicationYear();
            return ss.str();
        }
    );
    return list_of_books;
};

std::vector<std::string> UseCasesImpl::GetBooksBy(const std::string& author_name) {
    std::vector<std::string> list_of_books;
    std::ranges::transform(
        books_.GetBooksBy(author_name),
        std::back_inserter(list_of_books),
        [](auto& book) -> std::string {
            std::stringstream ss;
            ss << book.GetTitle() << ", " << book.GetPublicationYear();
            return ss.str();
        }
    );
    return list_of_books;
};

}  // namespace app
