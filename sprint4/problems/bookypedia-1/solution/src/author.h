#pragma once
#include "tagged_uuid.h"

#include <string>
#include <vector>
#include <optional>

namespace domain {

namespace detail {
struct AuthorTag {};
}  // namespace detail

using AuthorId = util::TaggedUUID<detail::AuthorTag>;

class Author {
public:
    Author(AuthorId id, std::string name)
        : id_(std::move(id))
        , name_(std::move(name)) {
    }

    const AuthorId& GetId() const noexcept {
        return id_;
    }

    const std::string& GetName() const noexcept {
        return name_;
    }

private:
    AuthorId id_;
    std::string name_;
};

class AuthorRepository {
public:
    virtual void Save(const Author& author) = 0;
    virtual std::vector<Author> GetAllAuthors() = 0;
    virtual std::optional<Author> GetAuthorBy(const std::string& author_name) = 0;

protected:
    ~AuthorRepository() = default;
};

}  // namespace domain
