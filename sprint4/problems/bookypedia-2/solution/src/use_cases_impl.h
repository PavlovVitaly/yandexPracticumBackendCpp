#pragma once
#include "author_fwd.h"
#include "book_fwd.h"
#include "use_cases.h"

namespace app {

class UseCasesImpl : public UseCases {
public:
    explicit UseCasesImpl(domain::AuthorRepository& authors,
                        domain::BookRepository& books)
        : authors_{authors}
        , books_{books} {
    }

    void AddAuthor(const std::string& name) override;
    std::vector<std::string> GetAllAuthors() override;
    std::optional<std::string> GetAuthorIdBy(const std::string& author_name) override;
    void AddBook(const std::string& author_id, const std::string& title, int year) override;
    std::vector<std::string> GetAllBooks() override;
    std::vector<std::string> GetBooksBy(const std::string& author_name) override;
private:
    domain::AuthorRepository& authors_;
    domain::BookRepository& books_;
};

}  // namespace app
