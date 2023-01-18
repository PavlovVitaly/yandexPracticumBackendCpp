#include "postgres.h"

#include <string>
#include <pqxx/zview.hxx>
#include <pqxx/pqxx>

namespace postgres {

using namespace std::literals;
using pqxx::operator"" _zv;

void AuthorRepositoryImpl::Save(const domain::Author& author) {
    pqxx::work work_{connection_};
    work_.exec_params(R"(
INSERT INTO authors (id, name) VALUES ($1, $2)
ON CONFLICT (id) DO UPDATE SET name=$2;
)"_zv,
                      author.GetId().ToString(), author.GetName());
    work_.commit();
}

std::vector<domain::Author> AuthorRepositoryImpl::GetAllAuthors() {
    std::vector<domain::Author> authors;
    pqxx::read_transaction read_transaction_{connection_};
    auto query_text = "SELECT * FROM authors ORDER BY name ASC"_zv;
    for (auto [id, name] : read_transaction_.query<std::string, std::string>(query_text)) {
        authors.emplace_back(domain::AuthorId::FromString(id), name);
    }
    return authors;
}

std::optional<domain::Author> AuthorRepositoryImpl::GetAuthorBy(const std::string& author_name) {
    pqxx::read_transaction read_transaction_{connection_};
    auto query_text = "SELECT * FROM authors WHERE name=" + read_transaction_.quote(author_name);
    std::optional tmp_author = read_transaction_.query01<std::string, std::string>(query_text);
    if (tmp_author) {
        auto [id, name] = *tmp_author;
        return domain::Author(domain::AuthorId::FromString(id), name);
    };
    return std::nullopt;
};

void BookRepositoryImpl::Save(const domain::Book& book) {
    pqxx::work work_{connection_};
    std::string title = book.GetTitle();
    work_.exec_params(R"(
INSERT INTO books (id, author_id, title, publication_year) VALUES ($1, $2, $3, $4)
ON CONFLICT (id) DO UPDATE SET author_id=$2, title=$3, publication_year=$4;
)"_zv,
                      book.GetId().ToString(),
                      book.GetAuthorId().ToString(),
                      book.GetTitle(),
                      book.GetPublicationYear());
    work_.commit();
}

std::vector<domain::Book> BookRepositoryImpl::GetAllBooks() {
    std::vector<domain::Book> books;
    pqxx::read_transaction read_transaction_{connection_};
    auto query_text = "SELECT id, author_id, title, publication_year FROM books ORDER BY title ASC"_zv;
    for (auto [id, author_id, title, year] : read_transaction_.query<std::string, std::string, std::string, int>(query_text)) {
        books.emplace_back(
            domain::BookId::FromString(id),
            domain::AuthorId::FromString(author_id),
            title,
            year
        );
    }
    return books;
}

std::vector<domain::Book> BookRepositoryImpl::GetBooksBy(const std::string& author_name) {
    std::vector<domain::Book> books;
    pqxx::read_transaction read_transaction_{connection_};
    auto query_text = "SELECT id, author_id, title, publication_year FROM books WHERE author_id=(SELECT id FROM authors WHERE name="
        + read_transaction_.quote(author_name)
        + " LIMIT 1) ORDER BY publication_year ASC, title ASC";
    for (auto [id, author_id, title, year] : read_transaction_.query<std::string, std::string, std::string, int>(query_text)) {
        books.emplace_back(
            domain::BookId::FromString(id),
            domain::AuthorId::FromString(author_id),
            title,
            year
        );
    }
    return books;
}

Database::Database(pqxx::connection connection)
    : connection_{std::move(connection)} {
    pqxx::work work_{connection_};
    work_.exec(R"(
CREATE TABLE IF NOT EXISTS authors (
    id UUID CONSTRAINT author_id_constraint PRIMARY KEY,
    name varchar(100) UNIQUE NOT NULL
);
)"_zv);

    work_.exec(R"(
CREATE TABLE IF NOT EXISTS books (
    id UUID CONSTRAINT book_id_constraint PRIMARY KEY,
    author_id UUID NOT NULL,
    title varchar(100) NOT NULL,
    publication_year int NOT NULL
);
)"_zv);
    // ... создать другие таблицы

    // коммитим изменения
    work_.commit();
}

}  // namespace postgres